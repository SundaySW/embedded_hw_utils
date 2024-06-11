#pragma once

#include "uart_task.hpp"
#include "embedded_hw_utils/connectivity/impl/interface_port.hpp"
#include "uart_pack.hpp"

namespace connectivity::uart{

struct UartPort final: InterfacePort<UartHandleT, UartTask, uart_queue_size>{

    static void ErrorHandler(){
        assert(false);
    }

    void StartReading(){
        rx_pack_.setPending();
        HAL_UARTEx_ReceiveToIdle_DMA(handle_, rx_pack_.data().data(), rx_pack_.size());
    }

    void RxHandlerSize(uint16_t size){
        rx_pack_.setReady(size);
    }

    bool PopPack(Pack& pack){
        if(!rx_pack_.isReady())
            return false;
        pack = rx_pack_;
        StartReading();
        return true;
    }

protected:
    void TaskPreProcedure() final{
        switch (current_task_.Type()){
            case connectivity::transmit_receive:
                HAL_UART_Transmit_IT(handle_, current_task_.TxData(), current_task_.TxSize());
                HAL_UART_Receive_DMA(handle_, current_task_.RxData(), current_task_.RxSize());
                break;
            case connectivity::transmit:
                HAL_UART_Transmit_IT(handle_, current_task_.TxData(), current_task_.TxSize());
                break;
            case connectivity::receive:
                HAL_UART_Receive_DMA(handle_, current_task_.RxData(), current_task_.RxSize());
                break;
        }
    }

    void TaskPostProcedure() final{

    }
private:
    Pack rx_pack_;
};

}//namespace connectivity::uart