#pragma once

#include "uart_port.hpp"

#define UART_DRIVER_(handle) Uart_Driver::global().GetPort(handle)
#define UART_CLEAR_Q_(handle) Uart_Driver::global().GetPort(handle)->ClearQueue()

namespace connectivity{

struct Uart_Driver final: InterfaceDriver<UartPort, k_interface_cnt>{
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
        connectivity::Uart_Driver::global().TxHandler(huart);
    }
    void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart) {
        connectivity::Uart_Driver::global().RxHandler(huart);
    }
    void HAL_UART_ErrorCallback(UART_HandleTypeDef* huart) {
        connectivity::Uart_Driver::global().ErrorHandler(huart);
    }
}
