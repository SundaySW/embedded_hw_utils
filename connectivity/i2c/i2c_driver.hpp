#pragma once

#include "embedded_hw_utils/connectivity/i2c/impl/i2c_port.hpp"

#define I2C_PLACE_TASK(handle, args...)  \
                        connectivity::i2c::I2CDriver::global().GetPort(handle)->PlaceTask(args)
#define I2C_PLACE_TASK_CB(handle, lambda, args...)  \
                        connectivity::i2c::I2CDriver::global().GetPort(handle)->PlaceTask(args, task::CB(this, lambda))
#define I2C_PLACE_TASK_PTR(handle, ptr, lambda, args...)  \
                        connectivity::i2c::I2CDriver::global().GetPort(handle)->PlaceTask(args, task::CB(ptr, lambda))

#define I2C_driver_place_port_(port) connectivity::i2c::I2CDriver::global().PlacePort(port)
#define I2C_driver_(handle) I2CDriver::global().GetPort(handle)
#define I2C_clear_queue_() I2CDriver::global().GetPort(handle)->ClearQueue()

namespace connectivity::i2c{

struct I2CDriver final: InterfaceDriver<I2CPort, i2c_interface_cnt>{

    static I2CDriver& global(){
        static auto instance = I2CDriver();
        return instance;
    }
};

} //namespace connectivity

extern "C"{
    void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef* handler) {
        connectivity::i2c::I2CDriver::global().TxHandler(handler);
    }

    void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef* handler) {
        connectivity::i2c::I2CDriver::global().TxHandler(handler);
    }

    void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef* handler) {
        connectivity::i2c::I2CDriver::global().RxHandler(handler);
    }

    void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef* handler) {
        connectivity::i2c::I2CDriver::global().RxHandler(handler);
    }

    void HAL_I2C_ErrorCallback(I2C_HandleTypeDef* handler) {
        connectivity::i2c:: I2CDriver::global().ErrorHandler(handler);
    }
}