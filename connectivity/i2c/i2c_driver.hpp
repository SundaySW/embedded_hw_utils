#pragma once

#include "ranges"

#include "async_tim_tasks/async_tim_tasks.hpp"

#include "i2c_port.hpp"

#define I2C_DRIVER_(handle) I2CDriver::global().GetPort(handle)
#define I2C_CLEAR_Q_() I2CDriver::global().GetPort(handle)->ClearQueue()

namespace connectivity{

struct I2CDriver final: InterfaceDriver<I2CPort, 1>{

    static I2CDriver& global(){
        static auto instance = I2CDriver();
        return instance;
    }
};

} //namespace connectivity

extern "C"{
    void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef* handler) {
        connectivity::I2CDriver::global().TxHandler(handler);
    }

    void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef* handler) {
        connectivity::I2CDriver::global().TxHandler(handler);
    }

    void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef* handler) {
        connectivity::I2CDriver::global().RxHandler(handler);
    }

    void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef* handler) {
        connectivity::I2CDriver::global().RxHandler(handler);
    }

    void HAL_I2C_ErrorCallback(I2C_HandleTypeDef* handler) {
        connectivity:: I2CDriver::global().ErrorHandler(handler);
    }
}