#pragma once

#include "array"
#include "fdcan.h"
#include "impl/can_port.hpp"
#include "can_client.hpp"

#define CANDriver() connectivity::can::Driver::global()

#define CANDriver_register_client(expr) \
connectivity::can::Driver::global().RegisterClient(connectivity::can::Client(this, [](void* context, CanPack &can_pack){\
    auto self = static_cast<decltype(this)>(context);                                                                   \
    expr;                                                                                                               \
}))

namespace connectivity::can{

struct Driver{
    static Driver& global(){
        static Driver driver{};
        return driver;
    }

    void SetHandler(FDCAN_HandleTypeDef* handler){
        port_.SetHandler(handler);
    }

    auto RxData(){
        return rx_data_.data();
    }

    auto* Header(){
        return &header_;
    }

    void OnRx(){
        port_.OnRX(header_, rx_data_.data());
    }

    void SendMsg(Pack& pack){
        port_.Send(pack);
    }

    void SendMsg(Pack&& pack){
        port_.Send(std::forward<Pack>(pack));
    }

    void ErrorHandler(HAL_StatusTypeDef status){
        switch (status) {
            case HAL_ERROR:
            case HAL_BUSY:
            case HAL_TIMEOUT:
                break;
            case HAL_OK:
                break;
        }
    }

    void RegisterClient(Client&& client){
        if(auto it = std::ranges::find(clients_, false, &Client::HasContext); it != clients_.end())
            *it = client;
    }

private:
    Driver(){
        RUN_ASYNC_fast({self->PollPort();});
    }

    void PollPort()
    {
        auto optional_pack = port_.Read();
        if(optional_pack)
            SendAll(optional_pack.value());
        port_.Trans();
    }

    void SendAll(Pack& pack){
        for(auto& client: clients_)
            client(pack);
    }

    std::array<Client, 10> clients_;
    FDCAN_RxHeaderTypeDef header_;
    std::array<uint8_t, 8> rx_data_;
    Port port_;
};

}//namespace connectivity

extern "C"{
void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{
    if((RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) != RESET)
    {
        auto status = HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0,
                                             connectivity::can::Driver::global().Header(),
                                             connectivity::can::Driver::global().RxData());
        if(status)
            connectivity::can::Driver::global().ErrorHandler(status);
        connectivity::can::Driver::global().OnRx();
    }
}
}