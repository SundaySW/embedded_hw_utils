#pragma once

#include "i2c_fwd.hpp"
#include "i2c_task.hpp"

namespace connectivity{

struct I2CPort final: InterfacePort<I2CHandleT, I2CTask, i2c_queue_size>{

    void ErrorHandler(){}

protected:
    void TaskPreprocedure() override final{
        switch (current_task_.Type()){
            case utils::transmit_receive:
                HAL_I2C_Master_Transmit_DMA(handle_, current_task_.GetAddr(), current_task_.TxData(), current_task_.TxSize());
                HAL_I2C_Master_Receive_DMA(handle_, current_task_.GetAddr(), current_task_.RxData(), current_task_.RxSize());
                break;
            case utils::transmit:
                HAL_I2C_Master_Transmit_DMA(handle_, current_task_.GetAddr(), current_task_.TxData(), current_task_.TxSize());
                break;
            case utils::receive:
                HAL_I2C_Master_Receive_DMA(handle_, current_task_.GetAddr(), current_task_.RxData(), current_task_.RxSize());
                break;
        }
    }
    void TaskPostProcedure() override final{}
};

} //namespace connectivity