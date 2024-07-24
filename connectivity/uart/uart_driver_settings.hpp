#pragma once

namespace connectivity::uart{
    constexpr std::size_t uart_tx_buffer_size {300};
    constexpr std::size_t uart_interface_cnt {1};
    constexpr std::size_t uart_queue_size {15};
    constexpr std::size_t rx_storage_size {16};
    using UartHandleT = UART_HandleTypeDef*;
}