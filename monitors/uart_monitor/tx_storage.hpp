#pragma once

#include <array>
#include <cstdint>
#include <ranges>
#include <cstring>

namespace utils{
template<std::size_t storage_size>
struct TxStorage {
    auto& at(auto idx){
        return data_.at(idx);
    }
    auto dataView(auto size = storage_size){
        return std::views::counted( data_.begin(), size );
    }
    void Reset(){
        cursor_ = 0;
    }
    bool FitsInRange(std::size_t amount){
        return cursor_ + amount <= data_.size();
    }
    auto currentDataIt(){
        return std::next(data_.begin(), cursor_);
    }
    auto cursor(){
        return cursor_;
    }
    auto& data(){
        return data_;
    }
    auto dataPtr(){
        return data_.data();
    }
    std::size_t size(){
        return data_.size();
    }
    template<typename ...Args>
    void PlaceToStorage(Args&&... args){
        (PlaceValue(std::forward<Args>(args)), ...);
    }
    template<typename ...Args>
    void StoreBytes(Args&&... args){
        (StoreByte(std::forward<Args>(args)), ...);
    }
private:
    std::size_t cursor_{0};
    std::array<uint8_t, storage_size> data_{};

    void StoreByte(uint8_t byte){
        assert(FitsInRange(1));
        data_[cursor_++] = byte;
    }

    template<typename T, std::size_t N>
    void PlaceValue(T(&array)[N])
    {
        auto size = N - 1;
        assert(FitsInRange(size));
        std::memcpy(currentDataIt(), array, size);
        cursor_ += size;
    }

    template<typename T>
    void PlaceValue(T&& array)
        requires(std::is_bounded_array<T>::value)
    {
        auto size = array.size() - 1;
        assert(FitsInRange(size));
        std::memcpy(currentDataIt(), array.begin(), size);
        cursor_ += size;
    }

    void PlaceValue(float& num){
        assert(FitsInRange(sizeof(num)));
        if(num < 0){
            StoreByte('-');
            num = -num;
        }
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

    template<typename T>
    void PlaceValue(T&& num)
    {
        assert(FitsInRange(sizeof(T)));
        if constexpr(std::is_same_v<std::remove_reference_t<T>, char>){
            StoreByte(num);
            return;
        }
        auto ptr = std::bit_cast<char*>(std::next(data_.begin(), cursor_));
        auto bytes_written = std::sprintf(ptr, "%d", num);
        if(bytes_written > 0)
            cursor_ += bytes_written;
    }

    template<typename ...Arrays>
    void PlaceArraysToStorage(Arrays&&... arrays){
        auto add_to_storage = [&]<typename T, std::size_t N>(T(&array)[N]){
            PlaceArr(std::forward<decltype(array)>(array));
        };
        (add_to_storage(arrays), ...);
    }
};

}//namespace utils
