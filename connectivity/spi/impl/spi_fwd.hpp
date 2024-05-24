#pragma once

#include "stm32g4xx_hal_spi.h"
#include "embedded_hw_utils/connectivity/impl/interface_task.hpp"
#include "embedded_hw_utils/connectivity/impl/interface_port.hpp"
#include "embedded_hw_utils/connectivity/impl/interface_driver.hpp"

namespace connectivity::spi{
    constexpr std::size_t spi_buffer_size = 4;
    constexpr std::size_t spi_interface_cnt = 1;
    constexpr std::size_t spi_queue_size = 10;
    using SpiHandleT = SPI_HandleTypeDef*;
}