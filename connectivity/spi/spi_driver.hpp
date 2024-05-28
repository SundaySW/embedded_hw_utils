#pragma once

#include "embedded_hw_utils/connectivity/impl/interface_driver.hpp"
#include "embedded_hw_utils/connectivity/spi/impl/spi_port.hpp"

#define SPI_DRIVER_(handle) connectivity::spi::SPI_Driver::global().GetPort(handle)

#define SPI_DRIVER_PLACE_PORT(hspi) \
                connectivity::spi::SPI_Driver::global().PlacePort(hspi);
#define SPI_PLACE_TASK(handle, args...)  \
                connectivity::spi::SPI_Driver::global().GetPort(handle)->PlaceTask(args)
#define SPI_PLACE_TASK_CB(handle, lambda, args...)  \
                connectivity::spi::SPI_Driver::global().GetPort(handle)->PlaceTask(args, connectivity::CB(this, lambda))
#define SPI_PLACE_TASK_PTR(handle, ptr, lambda, args...)  \
                connectivity::spi::SPI_Driver::global().GetPort(handle)->PlaceTask(args, connectivity::CB(ptr, lambda))
#define SPI_CLEAR_Q_() \
                connectivity::spi::SPI_Driver::global().GetPort(handle)->ClearQueue()
namespace connectivity::spi{

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
        connectivity::spi::SPI_Driver::global().TxHandler(hspi);
    }

    void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef* hspi){
        connectivity::spi::SPI_Driver::global().RxHandler(hspi);
    }
}