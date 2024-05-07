#pragma once

#include "embedded_hw_utils/connectivity/spi/impl/spi_port.hpp"
#include "embedded_hw_utils/connectivity/spi/impl/spi_fwd.hpp"

#define SPI_DRIVER_(handle) SPI_Driver::global().GetPort(handle)
#define SPI_CLEAR_Q_() SPI_Driver::global().GetPort(handle)->ClearQueue()

namespace connectivity{

    struct SPI_Driver final: InterfaceDriver<SpiPort, spi_interface_cnt>{
        static SPI_Driver& global(){
            static auto instance = SPI_Driver();
            return instance;
        }

        template<typename ...Types>
        void PlaceTask(SpiHandleT handle, Types&& ...args){
            if(auto it = std::ranges::find(ports_, handle, &SpiPort::GetHandle); it != ports_.end())
                it->PlaceTask(std::forward<Types>(args)...);
        }
    };

}//namespace connectivity

extern "C"
{
    void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef* hspi){
        connectivity::SPI_Driver::global().TxHandler(hspi);
    }

    void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef* hspi){
        connectivity::SPI_Driver::global().RxHandler(hspi);
    }
}