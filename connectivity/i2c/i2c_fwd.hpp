#pragma once

#include "stm32g4xx_hal_i2c.h"

#define interface_count (5)
#define buffer_size (4)
#define task_q_size (20)

namespace connectivity{

    using I2CHandleT = I2C_HandleTypeDef;

    enum class ReqType{
        read,
        write,
        error
    };

    enum TaskType{
        transmit,
        receive
    };
}