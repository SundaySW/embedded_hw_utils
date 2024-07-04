#pragma once

#include <array>
#include <cstring>

namespace monitor{
struct Packet{
    static constexpr std::size_t kPayload_size = 8;
    static constexpr std::size_t kCRC_width = 2;
    static constexpr std::size_t kService_width = 1;
    static constexpr std::size_t kPackSize = kPayload_size + kCRC_width + kService_width;

    void PlaceData(auto data, auto size){
        for(int i = 0; i < size; i++)
            PlaceByteToStorage(data[i]);
    }

    void PlaceData(uint8_t byte){
        PlaceByteToStorage(byte);
    }

    void Reset(){
        cursor_ = 0;
        crc_pass_ = false;
    }

    [[nodiscard]] const auto& data() const{ return storage_; }
    [[nodiscard]] std::size_t size() const{ return storage_.size(); }
    [[nodiscard]] bool isReady() const{ return crc_pass_;}
private:
    bool crc_pass_{false};
    std::size_t dlc_{0};
    std::size_t cursor_{0};
    std::array<uint8_t, kPackSize> storage_{};

    void PlaceByteToStorage(uint8_t byte){
        if(!isFull())
            storage_[cursor_++] = byte;
        if(isFull())
            CheckCRC();
    }

    void CheckCRC(){
        dlc_ = storage_.front();
        auto expected_crc = GetCRCFromPack();
        auto calc_crc = CalcCRC();

        if(expected_crc == calc_crc)
            crc_pass_ = true;
        else{
            crc_pass_ = false;
            Reset();
        }
    }

    uint16_t GetCRCFromPack(){
        auto it = storage_.end();
        auto f_b = std::prev(it, 1);
        auto s_b = std::prev(it, 2);
        return (*f_b << 8) | *s_b;
    }

    [[nodiscard]] bool isFull() const{
        return cursor_ == kPackSize;
    }

    [[nodiscard]]uint16_t CalcCRC() const{
        uint16_t crc = 0;
        auto payload_section_it = std::next(storage_.begin(), kService_width);
        for(std::size_t i = 0; i < kPayload_size; ++i, payload_section_it++)
            crc += *payload_section_it;
        crc = ((~crc + 1) & 0xffff);
        return crc;
    }
};
}// namespace monitor