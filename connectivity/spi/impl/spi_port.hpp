#pragma once

#include "spi_task.hpp"
#include "embedded_hw_utils/connectivity/impl/interface_port.hpp"

namespace connectivity::spi{

struct Port final: InterfacePort<HandleT, Task, queue_size>{
//        explicit Port(HandleT handler)
//            :InterfacePort(handler)
//        {}

    void ErrorHandler(){}

    protected:
        void TaskPreProcedure() override final{
            current_task_.ChipSelect();
            switch (current_task_.Type()){
                case connectivity::transmit_receive:
                    HAL_SPI_Transmit_DMA(handle_, current_task_.TxData(), current_task_.TxSize());
                    HAL_SPI_Receive_DMA(handle_, current_task_.RxData(), current_task_.RxSize());
                    break;
                case connectivity::transmit:
                    HAL_SPI_Transmit_DMA(handle_, current_task_.TxData(), current_task_.TxSize());
                    break;
                case connectivity::receive:
                    HAL_SPI_Receive_DMA(handle_, current_task_.RxData(), current_task_.RxSize());
                    break;
            }
        }
        void TaskPostProcedure() override final{
            current_task_.ChipRelease();
        }
    };

} //namespace connectivity::spi