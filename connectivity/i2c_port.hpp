#pragma once

#include "connectivity.hpp"

namespace connectivity{
struct I2CPort{
    constexpr I2C_HandleTypeDef* operator()() const{
        return i2c_handle_;
    }
    void HandleReq(ReqType type){

    }
    I2C_HandleTypeDef* GetHandle(){
        return i2c_handle_;
    }
    void SetHandle(I2C_HandleTypeDef* handle){
        i2c_handle_ = handle;
    }
private:
    I2C_HandleTypeDef* i2c_handle_{nullptr};
};
}