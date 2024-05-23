#pragma once

#include "array"
#include "fdcan.h"
#include "impl/can_port.hpp"

#define CAN_driver_() connectivity::CanDriver::global()

namespace connectivity{

struct CanDriver{
    static CanDriver& global(){
        static CanDriver driver{};
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
    void SendMsg(CANPack& pack){
        port_.Send(pack);
    }
    void SendMsg(CANPack&& pack){
        port_.Send(std::forward<CANPack>(pack));
    }
    void PollPort()
    {
        auto optional_pack = port_.Read();
        if (optional_pack)
            SendAll(optional_pack.value());
        port_.Trans();
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
    void SendAll(CANPack& pack){

    }

private:
    CanDriver(){
        PLACE_ASYNC_QUICKEST(task::CB(this, [](void* context){
            static_cast<CanDriver*>(context)->PollPort();
        }));
    }
    FDCAN_RxHeaderTypeDef header_;
    std::array<uint8_t, 8> rx_data_;
    CANPort port_;
};

}//namespace connectivity

extern "C"{
void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{
    if((RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) != RESET)
    {
        auto status = HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0,
                                             connectivity::CanDriver::global().Header(),
                                             connectivity::CanDriver::global().RxData());
        if(status)
            connectivity::CanDriver::global().ErrorHandler(status);
        connectivity::CanDriver::global().OnRx();
    }
}
}
