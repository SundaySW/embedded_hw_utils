#pragma once

#include "ranges"

#include "async_tim_tasks/async_tim_tasks.hpp"

#include "embedded_hw_utils/connectivity/i2c/impl/i2c_port.hpp"

#define I2C_driver_place_port_(port) connectivity::I2CDriver::global().PlacePort(port)
#define I2C_driver_(handle) I2CDriver::global().GetPort(handle)
#define I2C_clear_queue_() I2CDriver::global().GetPort(handle)->ClearQueue()

namespace connectivity{

struct I2CDriver final: InterfaceDriver<I2CPort, i2c_interface_cnt>{

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