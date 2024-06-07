#pragma once

#include <cstdint>
#include <array>
#include <span>
#include <cstring>

namespace connectivity::can {
struct Pack {
    enum {
        kPack_size = 8
    };

    Pack() = default;

    Pack(uint32_t id_, uint8_t dlc_, uint8_t *const data_)
        : id(id_), dlc(dlc_)
    {
        std::memcpy(data.data(), data_, dlc_);
    }

    Pack(uint32_t id_, std::span<uint8_t> data_)
        : id(id_)
    {
        std::memcpy(data.data(), data_.data(), data_.size());
    }

    uint32_t id{0};
    uint8_t dlc{0};
    std::array<uint8_t, kPack_size> data;
};
}