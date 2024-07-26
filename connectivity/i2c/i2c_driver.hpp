#pragma once

#include "embedded_hw_utils/connectivity/i2c/impl/i2c_port.hpp"

#define $I2CPlaceTask_cb(context_ptr, args...)    \
    DRIVER_place_task_cb(i2c, args)

namespace connectivity::i2c{

struct Driver final: InterfaceDriver<Port, interface_cnt>{

    static Driver& global(){
        static auto instance = Driver();
        return instance;
    }
};

template <typename ... Types>
static void PlaceTask(HandleT handle, Types&&... args){
    Driver::global().GetPort(handle)->PlaceTask(std::forward<Types>(args)...);
}

static void PlacePort(HandleT handle){
    Driver::global().PlacePort(handle);
}

static auto Port(HandleT handle){
    return Driver::global().GetPort(handle);
}

}//namespace connectivity::i2c

extern "C"{
    void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef* handler) {
        connectivity::i2c::Driver::global().TxHandler(handler);
    }

    void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef* handler) {
        connectivity::i2c::Driver::global().TxHandler(handler);
    }

    void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef* handler) {
        connectivity::i2c::Driver::global().RxHandler(handler);
    }

    void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef* handler) {
        connectivity::i2c::Driver::global().RxHandler(handler);
    }

    void HAL_I2C_ErrorCallback(I2C_HandleTypeDef* handler) {
        connectivity::i2c:: Driver::global().ErrorHandler(handler);
    }
}