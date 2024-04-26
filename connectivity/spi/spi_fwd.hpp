#pragma once

#include "embedded_hw_utils/IO/pin.hpp"
#include "embedded_hw_utils/utils/task_queue.hpp"

#include "async_tim_tasks/async_tim_tasks.hpp"

#include "stm32g4xx_hal_spi.h"
//#include "function2/function2.hpp"

#define SPI_DRIVER_ SPI_Driver::global()
#define SPI_CLEAR_Q_() SPI_Driver::global().ClearQueue()
#define SPI_POLL() SPI_Driver::global().ProcessTask()

//#define call_back_capacity (32U)
#define task_q_size (20)

namespace connectivity{
    using SpiHandleT = SPI_HandleTypeDef;
//    using CallBackT = fu2::function_base<true, false, fu2::capacity_fixed<call_back_capacity>,
//            false, false, void()>;
    using CallBackT = std::function<void(uint8_t*)>;

    enum TaskType{
        transmit,
        receive,
        transmit_receive
    };
}