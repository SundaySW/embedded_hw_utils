#pragma once

#include "stm32g4xx_hal_i2c.h"

namespace connectivity::i2c{
    constexpr std::size_t i2c_buffer_size = 4;
    constexpr std::size_t i2c_interface_cnt = 1;
    constexpr std::size_t i2c_queue_size = 8;

    using I2CHandleT = I2C_HandleTypeDef*;
    using I2CAddrT = uint16_t;
}