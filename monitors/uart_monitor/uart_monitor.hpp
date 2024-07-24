#pragma once

#include <ranges>

#include "embedded_hw_utils/utils/crc_packet.hpp"
#include "embedded_hw_utils/utils/tx_storage.hpp"

#include "embedded_hw_utils/connectivity/uart/uart_driver.hpp"
#include "async_tim_tasks/async_tim_tasks.hpp"

template<typename Monitor>
struct UartMonitor{
    using Packet = utils::Packet<8, 1>;
    using TxStorage = utils::TxStorage<connectivity::uart::uart_tx_buffer_size>;
protected:
    UartMonitor(UART_HandleTypeDef* uart_h, Monitor* monitor)
        :uart_handle_(uart_h)
        ,monitor_(monitor)
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
    Monitor* monitor_;
    UART_HandleTypeDef* uart_handle_;
    TxStorage tx_storage_;
    connectivity::uart::UartPort::RxStorage rx_storage_;
    Packet assembled_packet_;

    void HandleUartMsg(){
        auto size = rx_storage_.getRxSize();
        if(size == Packet::pack_size)
            assembled_packet_.Reset();

        assembled_packet_.PlaceData(rx_storage_.dataView(size));

        if(assembled_packet_.isReady()){
            monitor_->ProcessPacket(assembled_packet_.GetPayloadView());
            assembled_packet_.Reset();
        }
    }

    void SendToUART(){
        PlaceTermination();
        UART_PLACE_TASK(uart_handle_, utils::TxData{tx_storage_.dataPtr(), tx_storage_.cursor()});
        tx_storage_.Reset();
    }

    void Start(){
        UART_driver_(uart_handle_)->StartReading();
        RUN_ASYNC_hz({
            if(UART_driver_(self->uart_handle_)->GetPack(self->rx_storage_))
                self->HandleUartMsg();
        }, 100);
    }

    void PlaceTermination(){
        static unsigned char term = 0xFF;
        tx_storage_.StoreBytes(term, term, term);
    }
};