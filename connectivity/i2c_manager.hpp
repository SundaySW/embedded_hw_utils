#pragma once

#include "array"
#include "algorithm"

#include "connectivity.hpp"
#include "i2c_port.hpp"

#define Interface_Count (5)

namespace connectivity{

struct I2CManager{
    static I2CManager& global(){
        static auto instance = I2CManager();
        return instance;
    }
    I2CPort* SetPort(I2C_HandleTypeDef* i2c_handle){
        for(auto& slot: handlers_){
            if(slot.GetHandle() == nullptr){
                slot.SetHandle(i2c_handle);
                return &slot;
            }
        }
        return nullptr;
    }
    void HandleReq(I2C_HandleTypeDef* handle, ReqType type){
        if (auto it = std::ranges::find(handlers_, handle, &I2CPort::GetHandle); it != handlers_.end())
            it->HandleReq(type);
    }
private:
    std::array<I2CPort, Interface_Count> handlers_;
};

extern "C"{
    void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c) {
        I2CManager::global().HandleReq(hi2c, ReqType::write);
    }

    void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c) {
        I2CManager::global().HandleReq(hi2c, ReqType::read);
    }

    void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c) {
        I2CManager::global().HandleReq(hi2c, ReqType::error);
    }

    void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *hi2c) {
        I2CManager::global().HandleReq(hi2c, ReqType::write);
    }

    void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c) {
        I2CManager::global().HandleReq(hi2c, ReqType::read);
    }
}
} //namespace connectivity

