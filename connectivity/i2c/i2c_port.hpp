#pragma once

#include "i2c_fwd.hpp"
#include "i2c_task.hpp"

namespace connectivity{

struct I2CPort{
    constexpr I2CHandleT* operator()() const{
        return i2c_handle_;
    }
    void HandleReq(ReqType type){

    }
    I2CHandleT* GetHandle(){
        return i2c_handle_;
    }
    void SetHandle(I2CHandleT* handle){
        i2c_handle_ = handle;
    }
private:
    I2CHandleT* i2c_handle_{nullptr};
};
}