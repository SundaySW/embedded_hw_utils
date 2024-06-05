#pragma once

#include "uart_task.hpp"
#include "embedded_hw_utils/connectivity/impl/interface_port.hpp"

namespace connectivity::uart{

struct UartPort final: InterfacePort<UartHandleT, UartTask, uart_queue_size>{

    void ErrorHandler(){
        assert(false);
    }

protected:
    void TaskPreProcedure() override final{
        switch (current_task_.Type()){
            case connectivity::transmit_receive:
                HAL_UART_Transmit_DMA(handle_, current_task_.TxData(), current_task_.TxSize());
                HAL_UART_Receive_DMA(handle_, current_task_.RxData(), current_task_.RxSize());
                break;
            case connectivity::transmit:
                HAL_UART_Transmit_DMA(handle_, current_task_.TxData(), current_task_.TxSize());
                break;
            case connectivity::receive:
                HAL_UART_Receive_DMA(handle_, current_task_.RxData(), current_task_.RxSize());
                break;
        }
    }

    void TaskPostProcedure() override final{

    }
};

} //namespace connectivity