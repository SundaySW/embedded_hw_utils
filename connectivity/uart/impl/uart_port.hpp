#pragma once

#include "uart_task.hpp"
#include "uart_fwd.hpp"

namespace connectivity{


struct UartPort final: InterfacePort<UartHandleT, TaskT, uart_queue_size>{

    void ErrorHandler(){}

protected:
    void TaskPreprocedure() override final{
        switch (current_task_.Type()){
            case utils::transmit_receive:
                HAL_UART_Transmit_DMA(handle_, current_task_.TxData(), current_task_.TxSize());
                HAL_UART_Receive_DMA(handle_, current_task_.RxData(), current_task_.RxSize());
                break;
            case utils::transmit:
                HAL_UART_Transmit_DMA(handle_, current_task_.TxData(), current_task_.TxSize());
                break;
            case utils::receive:
                HAL_UART_Receive_DMA(handle_, current_task_.RxData(), current_task_.RxSize());
                break;
        }
    }

    void TaskPostProcedure() override final{}
};

} //namespace connectivity