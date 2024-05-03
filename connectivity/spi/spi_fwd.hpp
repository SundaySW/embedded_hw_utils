#pragma once

#include "stm32g4xx_hal_spi.h"
#include "embedded_hw_utils/utils/interface_task.hpp"
#include "embedded_hw_utils/utils/interface_port.hpp"
#include "embedded_hw_utils/utils/interface_driver.hpp"

namespace connectivity{
    constexpr std::size_t k_buffer_size = 4;
    constexpr std::size_t k_interface_cnt = 8;
    constexpr std::size_t k_queue_size = 20;
    using SpiHandleT = SPI_HandleTypeDef*;
}