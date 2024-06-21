#pragma once

#include "embedded_hw_utils/connectivity/uart/uart_driver.hpp"
#include "async_tim_tasks/async_tim_tasks.hpp"

struct UartMonitor{
    UartMonitor(UART_HandleTypeDef* uart_h)
        :uart_handle_(uart_h)
    {
        Start();
    }

protected:
    connectivity::uart::Pack pack_;

    virtual void HandleUartMsg() = 0;

    template<typename ...Args>
    void PlaceToStorage(Args&&... args){
        (PlaceValue(std::forward<Args>(args)), ...);
    }

    void SendToMonitor(){
        PlaceTermination();
        UART_PLACE_TASK(uart_handle_, utils::TxData{tx_storage_.data(), storage_cursor_});
        storage_cursor_ = 0;
    }

    template<typename ...Args>
    void SendToMonitor(Args&&... args){
        PlaceToStorage(std::forward<Args>(args)...);
        SendToMonitor();
    }

private:
    UART_HandleTypeDef* uart_handle_;
    std::array<uint8_t, connectivity::uart::uart_buffer_size> tx_storage_;
    std::size_t storage_cursor_ = 0;

    void Start(){
        UART_driver_(uart_handle_)->StartReading();
        PLACE_ASYNC_TASK({
            if(UART_driver_(self->uart_handle_)->GetPack(self->pack_))
                self->HandleUartMsg();
        }, 20);
    }

    template<typename T, std::size_t N>
    void PlaceValue(T(&array)[N])
    {
        auto size = N - 1;
        assert(storage_cursor_ + size <= tx_storage_.size());
        std::memcpy(tx_storage_.begin() + storage_cursor_, array, size);
        storage_cursor_ += size;
    }

    template<typename T>
    void PlaceValue(T&& num)
    {
        auto ptr = std::bit_cast<char*>(tx_storage_.data()) + storage_cursor_;
        auto bytes_written = std::sprintf(ptr, "%d", num);
        if(bytes_written > 0)
            storage_cursor_ += bytes_written;
    }

    void PlaceValue(float& num)
    {
//        auto ptr = std::bit_cast<char*>(tx_storage_.data()) + storage_cursor_;
//        auto bytes_written = std::sprintf(ptr, "%.2f", num);
//        if(bytes_written > 0)
//            storage_cursor_ += bytes_written;
        static constexpr float kPrecision = 0.1;
        int cursor = std::log10(num);
        int digit;
        while (num > kPrecision)
        {
            auto weight = std::pow(10.0f, cursor);
            digit = std::floor(num / weight);
            num -= digit * weight;
            StoreByte('0' + digit);
            if(cursor == 0)
                StoreByte('.');
            cursor--;
        }
        StoreByte('0');
    }

//    template<typename T>
//    void PlaceNumToStorage(T&& num)
//    {
//        static constexpr float kPrecision = 0.001;
//        int cursor = std::log10(num);
//        int digit;
//        while (num > kPrecision)
//        {
//            auto weight = std::pow(10.0f, cursor);
//            digit = std::floor(num / weight);
//            num -= digit * weight;
//            StoreByte('0' + digit);
//            if constexpr(std::is_floating_point<T>::value) {
//                if(cursor == 0)
//                    StoreByte('.');
//            }
//            cursor--;
//        }
//    }

    void PlaceTermination(){
        StoreBytes(0xFF, 0xFF, 0xFF);
    }

    template<typename ...Arrays>
    void PlaceArraysToStorage(Arrays&&... arrays){
        auto add_to_storage = [&]<typename T, std::size_t N>(T(&array)[N]){
            auto size = N - 1;
            assert(storage_cursor_ + size <= tx_storage_.size());
            std::memcpy(tx_storage_.begin() + storage_cursor_, array, size);
            storage_cursor_ += size;
        };
        (add_to_storage(arrays), ...);
    }

    template<typename ...Args>
    void StoreBytes(Args&&... args){
        (StoreByte(std::forward<Args>(args)), ...);
    }

    void StoreByte(uint8_t c){
        if(connectivity::uart::uart_buffer_size - storage_cursor_ < 1)
            return;
        tx_storage_.at(storage_cursor_++) = c;
    }
};