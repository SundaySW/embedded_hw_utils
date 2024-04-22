#pragma once

#include "array"
#include "algorithm"

#include "i2c_fwd.hpp"

#include "i2c_port.hpp"

namespace connectivity{

struct I2CManager{

    static I2CManager& global(){
        static auto instance = I2CManager();
        return instance;
    }

    I2CPort* SetPort(I2CHandleT* i2c_handle){
        for(auto& slot: handlers_){
            if(slot.GetHandle() == nullptr){
                slot.SetHandle(i2c_handle);
                return &slot;
            }
        }
        return nullptr;
    }

    void HandleReq(I2CHandleT* handle, ReqType type){
        if (auto it = std::ranges::find(handlers_, handle, &I2CPort::GetHandle); it != handlers_.end())
            it->HandleReq(type);
    }

private:
    std::array<I2CPort, interface_count> handlers_;
};

extern "C"{
    void HAL_I2C_MasterTxCpltCallback(I2CHandleT *hi2c) {
        I2CManager::global().HandleReq(hi2c, ReqType::write);
    }

    void HAL_I2C_MasterRxCpltCallback(I2CHandleT *hi2c) {
        I2CManager::global().HandleReq(hi2c, ReqType::read);
    }

    void HAL_I2C_ErrorCallback(I2CHandleT *hi2c) {
        I2CManager::global().HandleReq(hi2c, ReqType::error);
    }

    void HAL_I2C_MemTxCpltCallback(I2CHandleT *hi2c) {
        I2CManager::global().HandleReq(hi2c, ReqType::write);
    }

    void HAL_I2C_MemRxCpltCallback(I2CHandleT *hi2c) {
        I2CManager::global().HandleReq(hi2c, ReqType::read);
    }
}
} //namespace connectivity

