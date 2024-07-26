#pragma once

#include "embedded_hw_utils/connectivity/impl/interface_driver.hpp"
#include "embedded_hw_utils/connectivity/spi/impl/spi_port.hpp"

#define $SPIPlaceTask_cb(context_ptr, handle, expr, args...)   \
    DRIVER_place_task_cb(spi, args)

namespace connectivity::spi{

struct Driver final: InterfaceDriver<Port, interface_cnt>{
    static Driver& global(){
        static auto instance = Driver();
        return instance;
    }
};

template <typename ... Types>
static void PlaceTask(HandleT handle, Types&&... args){
    Driver::global().GetPort(handle)->PlaceTask(std::forward<Types>(args)...);
}

void PlacePort(HandleT handle){
    Driver::global().PlacePort(handle);
}

auto Port(HandleT handle){
    return Driver::global().GetPort(handle);
}

}//namespace connectivity::spi

extern "C"
{
    void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef* hspi){
        connectivity::spi::Driver::global().TxHandler(hspi);
    }

    void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef* hspi){
        connectivity::spi::Driver::global().RxHandler(hspi);
    }
}