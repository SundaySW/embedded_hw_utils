#pragma once

#include "spi_fwd.hpp"

#include "embedded_hw_utils/utils/task.hpp"

namespace connectivity{

struct SpiTask{
    void PlaceData(auto data_size_pair){
        auto&& [tx_data_p, size] = data_size_pair;
        if(size <= sizeof tx_data_)
            std::memcpy(tx_data_.data(), tx_data_p, size);
//            std::copy(tx_data_p, tx_data_p + size, tx_data_.begin());
    }
    void CallBack(){
        if(call_back_)
            call_back_.value()(rx_data_.data());
    }
    void ChipRelease(){
        if(pin_)
            pin_.value()->setValue(PIN_BOARD::HIGH);
    }
    void ChipSelect(){
        if(pin_)
            pin_.value()->setValue(PIN_BOARD::LOW);
    }
    uint8_t* TxData(){ return tx_data_.data(); }
    uint8_t* RxData(){ return rx_data_.data(); }
    TaskType Type(){ return type_; }
    [[nodiscard]] std::size_t TxSize() const { return tx_size_; }
    [[nodiscard]] std::size_t RxSize() const { return rx_size_; }

    SpiTask() = default;
    SpiTask(std::pair<const uint8_t*, std::size_t> data_size_pair, TaskType type = transmit)
            :tx_size_(data_size_pair.second)
            ,type_(type)
    {
        PlaceData(data_size_pair);
    }
    SpiTask(std::pair<const uint8_t*, std::size_t> data_size_pair, std::size_t rx_size, PIN_BOARD::PIN<PIN_BOARD::PinWriteable>* pin, CallBackT call_back, TaskType type = transmit_receive)
            :pin_(pin)
            ,call_back_(std::move(call_back))
            ,rx_size_(rx_size)
            ,tx_size_(data_size_pair.second)
            ,type_(type)
    {
        PlaceData(data_size_pair);
    }
    SpiTask(std::pair<const uint8_t*, std::size_t> data_size_pair, std::size_t rx_size, CallBackT call_back, TaskType type = transmit_receive)
            :call_back_(std::move(call_back))
            ,rx_size_(rx_size)
            ,tx_size_(data_size_pair.second)
            ,type_(type)
    {
        PlaceData(data_size_pair);
    }
    SpiTask(std::size_t rx_size, PIN_BOARD::PIN<PIN_BOARD::PinWriteable>* pin, CallBackT call_back, TaskType type = receive)
            :pin_(pin)
            ,call_back_(std::move(call_back))
            ,rx_size_(rx_size)
            ,type_(type)
    {}
    SpiTask(std::pair<const uint8_t*, std::size_t> data_size_pair, PIN_BOARD::PIN<PIN_BOARD::PinWriteable>* pin, TaskType type = transmit)
            :pin_(pin)
            ,tx_size_(data_size_pair.second)
            ,type_(type)
    {
        PlaceData(data_size_pair);
    }
    SpiTask(std::pair<const uint8_t*, std::size_t> data_size_pair, CallBackT call_back, TaskType type = transmit)
            :call_back_(std::move(call_back))
            ,tx_size_(data_size_pair.second)
            ,type_(type)
    {
        PlaceData(data_size_pair);
    }
    SpiTask(std::pair<const uint8_t*, std::size_t> data_size_pair, PIN_BOARD::PIN<PIN_BOARD::PinWriteable>* pin, CallBackT call_back, TaskType type = transmit)
            :pin_(pin)
            ,call_back_(std::move(call_back))
            ,tx_size_(data_size_pair.second)
            ,type_(type)
    {
        PlaceData(data_size_pair);
    }
private:
    std::optional<PIN_BOARD::PIN<PIN_BOARD::PinWriteable>*> pin_;
    TaskType type_;
    std::array<uint8_t, buffer_size> rx_data_{0,};
    std::array<uint8_t, buffer_size> tx_data_{0,};
    std::size_t rx_size_;
    std::size_t tx_size_;
    std::optional<CallBackT> call_back_;
};
}//namespace connectivity