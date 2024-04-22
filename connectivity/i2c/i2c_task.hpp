#pragma once

#include <cstring>
#include "array"

#include "i2c_fwd.hpp"

namespace connectivity{

    struct I2CTask{
        void PlaceData(auto data_size_pair){
            auto&& [tx_data_p, size] = data_size_pair;
            if(size <= data_.size())
                std::memcpy(data_.data(), tx_data_p, size);
//            std::copy(tx_data_p, tx_data_p + size, tx_data_.begin());
        }
        void CallBack(){
            if(call_back_)
                call_back_.value()(data_.data());
        }

        uint8_t* Data() { return data_.data(); }
        uint8_t DataSize() const {return data_size_;}
        TaskType Type(){ return type_; }

        I2CTask() = default;
        I2CTask(std::pair<const uint8_t*, std::size_t> data_size_pair, TaskType type = transmit)
                :tx_size_(data_size_pair.second)
                ,type_(type)
        {
            PlaceData(data_size_pair);
        }
        I2CTask(std::pair<const uint8_t*, std::size_t> data_size_pair, std::size_t rx_size, PIN_BOARD::PIN<PIN_BOARD::PinWriteable>* pin, CallBackT call_back, TaskType type = transmit_receive)
                :pin_(pin)
                ,call_back_(std::move(call_back))
                ,rx_size_(rx_size)
                ,tx_size_(data_size_pair.second)
                ,type_(type)
        {
            PlaceData(data_size_pair);
        }
        I2CTask(std::pair<const uint8_t*, std::size_t> data_size_pair, std::size_t rx_size, CallBackT call_back, TaskType type = transmit_receive)
                :call_back_(std::move(call_back))
                ,rx_size_(rx_size)
                ,tx_size_(data_size_pair.second)
                ,type_(type)
        {
            PlaceData(data_size_pair);
        }
        I2CTask(std::size_t rx_size, PIN_BOARD::PIN<PIN_BOARD::PinWriteable>* pin, CallBackT call_back, TaskType type = receive)
                :pin_(pin)
                ,call_back_(std::move(call_back))
                ,rx_size_(rx_size)
                ,type_(type)
        {}
        I2CTask(std::pair<const uint8_t*, std::size_t> data_size_pair, PIN_BOARD::PIN<PIN_BOARD::PinWriteable>* pin, TaskType type = transmit)
                :pin_(pin)
                ,tx_size_(data_size_pair.second)
                ,type_(type)
        {
            PlaceData(data_size_pair);
        }
        I2CTask(std::pair<const uint8_t*, std::size_t> data_size_pair, CallBackT call_back, TaskType type = transmit)
                :call_back_(std::move(call_back))
                ,tx_size_(data_size_pair.second)
                ,type_(type)
        {
            PlaceData(data_size_pair);
        }
        I2CTask(std::pair<const uint8_t*, std::size_t> data_size_pair, PIN_BOARD::PIN<PIN_BOARD::PinWriteable>* pin, CallBackT call_back, TaskType type = transmit)
                :pin_(pin)
                ,call_back_(std::move(call_back))
                ,tx_size_(data_size_pair.second)
                ,type_(type)
        {
            PlaceData(data_size_pair);
        }
    private:
        TaskType type_;
        std::array<uint8_t, buffer_size> data_{0,};
        uint8_t data_size_{0};
        std::optional<CallBackT> call_back_;
    };
}//namespace connectivity