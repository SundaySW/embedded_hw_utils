#pragma once

#include "embedded_hw_utils/connectivity/uart/impl/uart_port.hpp"
#include "embedded_hw_utils/connectivity/impl/interface_driver.hpp"

#define UART_PLACE_TASK(handle, args...)  \
            connectivity::uart::Uart_Driver::global().GetPort(handle)->PlaceTask(args)
#define UART_PLACE_TASK_CB(handle, lambda, args...)  \
            connectivity::uart::Uart_Driver::global().GetPort(handle)->PlaceTask(args, connectivity::CB(this, lambda))
#define UART_PLACE_TASK_PTR(handle, ptr, lambda, args...)  \
            connectivity::uart::Uart_Driver::global().GetPort(handle)->PlaceTask(args, connectivity::CB(ptr, lambda))

#define UART_driver_place_port_(port) connectivity::uart::Uart_Driver::global().PlacePort(port)
#define UART_driver_(handle) connectivity::uart::Uart_Driver::global().GetPort(handle)
#define UART_clear_queue_(handle) connectivity::uart::Uart_Driver::global().GetPort(handle)->ClearQueue()

namespace connectivity::uart{

struct Uart_Driver final: InterfaceDriver<UartPort, uart_interface_cnt>{
    static Uart_Driver& global(){
        static auto instance = Uart_Driver();
        return instance;
    }

    template<typename ...Types>
    void PlaceTask(UartHandleT handle, Types&& ...args){
        if(auto it = std::ranges::find(ports_, handle, &UartPort::GetHandle); it != ports_.end())
            it->PlaceTask(std::forward<Types>(args)...);
    }
};

}//namespace connectivity

extern "C"
{
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
