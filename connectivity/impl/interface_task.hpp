#pragma once

#include <cstring>
#include <array>
#include <functional>
#include <cstdint>
#include <optional>

#include "embedded_hw_utils/utils/task_callback.hpp"

namespace utils{
    using CallBackT = task::CallBack<uint8_t*>;

    enum TaskType{
        transmit,
        receive,
        transmit_receive
    };

    template<std::size_t buffer_size = 8>
    struct Task{
//        void PlaceData(auto data_size_pair){
//            auto&& [tx_data_p, size] = data_size_pair;
//            if(size <= sizeof tx_data_)
//                std::memcpy(tx_data_.data(), tx_data_p, size);
//        }

        void PlaceData(std::span<uint8_t> data){
            if(data.size() <= sizeof tx_data_)
                std::memcpy(tx_data_.data(), data.data(), data.size());
        }

        void CallBack(){
            if(call_back_)
                call_back_.value()(rx_data_.data());
        }

        uint8_t* TxData(){ return tx_data_.data(); }
        uint8_t* RxData(){ return rx_data_.data(); }
        TaskType Type(){ return type_; }
        [[nodiscard]] std::size_t TxSize() const { return tx_size_; }
        [[nodiscard]] std::size_t RxSize() const { return rx_size_; }

        Task() = default;
        Task(std::span<uint8_t> data)
                :tx_size_(data.size())
                ,type_(transmit)
        {
            PlaceData(data);
        }

        Task(std::span<uint8_t> data, CallBackT call_back)
            :call_back_(std::move(call_back))
            ,tx_size_(data.size())
            ,type_(transmit)
        {
            PlaceData(data);
        }

        Task(std::size_t rx_size, CallBackT call_back)
            :call_back_(std::move(call_back))
            ,rx_size_(rx_size)
            ,type_(receive)
        {}

        Task(std::size_t rx_size)
            :rx_size_(rx_size)
            ,type_(receive)
        {}

        Task(std::span<uint8_t> data, std::size_t rx_size, CallBackT call_back)
            :call_back_(std::move(call_back))
            ,rx_size_(rx_size)
            ,tx_size_(data.size())
            ,type_(transmit_receive)
        {
            PlaceData(data);
        }

    private:
        TaskType type_;
        std::array<uint8_t, buffer_size> rx_data_{0,};
        std::array<uint8_t, buffer_size> tx_data_{0,};
        std::size_t rx_size_;
        std::size_t tx_size_;
        std::optional<CallBackT> call_back_;
    };
}