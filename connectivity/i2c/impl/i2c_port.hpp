#pragma once

#include "i2c_task.hpp"

namespace connectivity::i2c{

struct Port final: InterfacePort<HandleT, Task, queue_size>{

void ErrorHandler(){}

protected:
    void TaskPreProcedure() override final{
        switch (current_task_.Type()){
            case connectivity::transmit_receive:
                HAL_I2C_Master_Transmit_DMA(handle_, current_task_.GetAddr(), current_task_.TxData(), current_task_.TxSize());
                HAL_I2C_Master_Receive_DMA(handle_, current_task_.GetAddr(), current_task_.RxData(), current_task_.RxSize());
                break;
            case connectivity::transmit:
                HAL_I2C_Master_Transmit_DMA(handle_, current_task_.GetAddr(), current_task_.TxData(), current_task_.TxSize());
                break;
            case connectivity::receive:
                HAL_I2C_Master_Receive_DMA(handle_, current_task_.GetAddr(), current_task_.RxData(), current_task_.RxSize());
                break;
        }
    }
    void TaskPostProcedure() override final{}
};

} //namespace connectivity