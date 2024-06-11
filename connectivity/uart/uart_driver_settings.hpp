#pragma once

namespace connectivity::uart{
    constexpr std::size_t uart_buffer_size {80};
    constexpr std::size_t uart_interface_cnt {1};
    constexpr std::size_t uart_queue_size {15};
    constexpr std::size_t kPack_size {128};
    using UartHandleT = UART_HandleTypeDef*;
}