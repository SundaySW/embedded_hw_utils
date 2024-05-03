#pragma once

#include "stm32g4xx_hal_i2c.h"
#include "embedded_hw_utils/utils/interface_task.hpp"
#include "embedded_hw_utils/utils/interface_port.hpp"
#include "embedded_hw_utils/utils/interface_driver.hpp"

namespace connectivity{
    constexpr std::size_t k_buffer_size = 8;
    constexpr std::size_t k_interface_cnt = 8;
    constexpr std::size_t k_queue_size = 8;

    using I2CHandleT = I2C_HandleTypeDef*;
    using I2CAddrT = uint16_t;
}