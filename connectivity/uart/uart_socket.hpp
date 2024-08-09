#pragma once

#include <ranges>

#include "embedded_hw_utils/utils/crc_packet.hpp"
#include "tx_storage.hpp"

#include "embedded_hw_utils/connectivity/uart/uart_driver.hpp"
#include "async_tim_tasks/async_tim_tasks.hpp"

namespace connectivity::uart{

template<typename User>
struct Socket{
    using Packet = utils::Packet<8, 1>;
    using TxStorage_t = TxStorage<tx_buffer_size>;

    Socket(HandleT uart_h, User* user)
        : uart_handle_(uart_h)
        , user_(user)
    {
        Start();
    }

    template<typename ...Args>
    void PlaceAndSend(Args&&... args){
        tx_storage_.PlaceToStorage(std::forward<Args>(args)...);
        SendToUART();
    }

    template<typename ...Args>
    void Place(Args&&... args){
        tx_storage_.PlaceToStorage(std::forward<Args>(args)...);
    }

private:
    User* user_;
    HandleT uart_handle_;
    TxStorage_t tx_storage_;
    Port::RxStorage rx_storage_;
    Packet assembled_packet_;

    void HandleUartMsg(){
        auto size = rx_storage_.getRxSize();
        if(size == Packet::pack_size)
            assembled_packet_.Reset();

        assembled_packet_.PlaceData(rx_storage_.dataView(size));

        if(assembled_packet_.isReady()){
            user_->ProcessPacket(assembled_packet_.GetPayloadView());
            assembled_packet_.Reset();
        }
    }

    void SendToUART(){
        PlaceTask(uart_handle_, utils::TxData{tx_storage_.dataPtr(), tx_storage_.cursor()});
        tx_storage_.Reset();
    }

    void Start(){
        Port(uart_handle_)->StartReading();
        $RunAsync({
            if(Port(self->uart_handle_)->GetPack(self->rx_storage_))
                self->HandleUartMsg();
        }, 100);
    }
};

}//namespace connectivity::uart