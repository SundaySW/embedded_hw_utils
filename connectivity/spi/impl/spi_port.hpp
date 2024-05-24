#pragma once

#include "spi_task.hpp"
#include "spi_fwd.hpp"

namespace connectivity::spi{

struct SpiPort final: InterfacePort<SpiHandleT, SpiTask, spi_queue_size>{
//        explicit SpiPort(SpiHandleT handler)
//            :InterfacePort(handler)
//        {}

    void ErrorHandler(){}

    protected:
        void TaskPreprocedure() override final{
            current_task_.ChipSelect();
            switch (current_task_.Type()){
                case utils::transmit_receive:
                    HAL_SPI_Transmit_DMA(handle_, current_task_.TxData(), current_task_.TxSize());
                    HAL_SPI_Receive_DMA(handle_, current_task_.RxData(), current_task_.RxSize());
                    break;
                case utils::transmit:
                    HAL_SPI_Transmit_DMA(handle_, current_task_.TxData(), current_task_.TxSize());
                    break;
                case utils::receive:
                    HAL_SPI_Receive_DMA(handle_, current_task_.RxData(), current_task_.RxSize());
                    break;
            }
        }
        void TaskPostProcedure() override final{
            current_task_.ChipRelease();
        }
    };

} //namespace connectivity