#pragma once

#include "embedded_hw_utils/connectivity/uart/impl/uart_port.hpp"
#include "embedded_hw_utils/connectivity/impl/interface_driver.hpp"

#define UART_PLACE_TASK(handle, args...)  \
            connectivity::uart::Uart_Driver::global().GetPort(handle)->PlaceTask(args)

#define UART_PLACE_TASK_CB(handle, expr, args...)  \
            connectivity::uart::Uart_Driver::global().GetPort(handle)->PlaceTask(args, connectivity::CB(this, [](void* context, uint8_t* data){ \
            auto self = static_cast<decltype(this)>(context);                                               \
            expr;                                                                                           \
        }))

#define UART_PLACE_TASK_PTR(handle, ptr, expr, args...)  \
        connectivity::uart::Uart_Driver::global().GetPort(handle)->PlaceTask(args, connectivity::CB(ptr, [](void* context, uint8_t* data){ \
            auto context_ptr = static_cast<decltype(this)>(context);                                               \
            expr;                                                                                           \
        }))

#define UARTDriver_place_port_(port) connectivity::uart::Uart_Driver::global().PlacePort(port)
#define UART_driver_(handle) connectivity::uart::Uart_Driver::global().GetPort(handle)
#define UART_clear_queue_(handle) connectivity::uart::Uart_Driver::global().GetPort(handle)->ClearQueue()

namespace connectivity::uart{

struct Uart_Driver final: InterfaceDriver<UartPort, uart_interface_cnt>{
    static Uart_Driver& global(){
        static auto instance = Uart_Driver();
        return instance;
    }
    void RxHandlerSize(UartHandleT handle, uint16_t size){
        if(auto it = std::ranges::find(ports_, handle, &UartPort::GetHandle); it != ports_.end())
            it->RxHandlerSize(size);
    }
};

}//namespace connectivity::uart

extern "C"
{
    void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t size) {
        connectivity::uart::Uart_Driver::global().RxHandlerSize(huart, size);
    }
    void HAL_UART_TxCpltCallback(UART_HandleTypeDef* huart) {
        connectivity::uart::Uart_Driver::global().TxHandler(huart);
    }
    void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart) {
        connectivity::uart::Uart_Driver::global().RxHandler(huart);
    }
    void HAL_UART_ErrorCallback(UART_HandleTypeDef* huart) {
        connectivity::uart::Uart_Driver::global().ErrorHandler(huart);
    }
}