#pragma once

#include <cstddef>

namespace utils{
    struct TxData{
        unsigned char* data_ptr;
        std::size_t size;
    };
}