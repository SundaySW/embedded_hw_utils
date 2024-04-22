#pragma once

#include "spi_fwd.hpp"
#include "spi_task.hpp"

#include <utility>
#include <optional>

namespace connectivity{

struct SPI_Driver{

    explicit SPI_Driver(SpiHandleT* hspi)
    {
        spi_handler_ = hspi;
        PLACE_ASYNC_QUICKEST([&]{
            SPI_POLL();
        });
    }

    static void SetHandler(SpiHandleT* spiHandle){
        spi_handler_ = spiHandle;
    }

    static auto GetHandler(){
        return spi_handler_;
    }

    void TxCallbackHandler(){
        if(current_task_.Type() == transmit_receive)
            return;
        FinishTask();
    }

    void RxCallbackHandler(){
        FinishTask();
    }

    void ClearQueue(){
        tasks_.clear();
        in_process_ = false;
    }

    void FinishTask(){
        if(in_process_){
            current_task_.CallBack();
            current_task_.ChipRelease();
            in_process_ = false;
        }
    }

    static SPI_Driver& global(){
        static auto instance = SPI_Driver(spi_handler_);
        return instance;
    }

    void ProcessTask(){
        if(!tasks_.empty() && !in_process_){
            in_process_ = true;
            current_task_ = tasks_.front();
            tasks_.pop();
            current_task_.ChipSelect();
            switch (current_task_.Type()){
                case transmit_receive:
                    HAL_SPI_Transmit_DMA(spi_handler_, current_task_.TxData(), current_task_.TxSize());
                    HAL_SPI_Receive_DMA(spi_handler_, current_task_.RxData(), current_task_.RxSize());
                    break;
                case transmit:
                    HAL_SPI_Transmit_DMA(spi_handler_, current_task_.TxData(), current_task_.TxSize());
                    break;
                case receive:
                    HAL_SPI_Receive_DMA(spi_handler_, current_task_.RxData(), current_task_.RxSize());
                    break;
            }
        }
    }

    void PlaceTask(auto&& ... args){
        tasks_.push(SpiTask(std::forward<decltype(args)>(args)...));
    }

    auto operator()(){
        return spi_handler_;
    }
private:
    bool in_process_{false};
    SpiTask current_task_;
    TaskQueue<SpiTask, task_q_size> tasks_;
    static inline SpiHandleT* spi_handler_;
};

extern "C"
{
    void HAL_SPI_TxCpltCallback(SpiHandleT *hspi) {
        if(hspi == SPI_Driver::global()())
            SPI_Driver::global().TxCallbackHandler();
    }

    void HAL_SPI_RxCpltCallback(SpiHandleT *hspi) {
        if(hspi == SPI_Driver::global()())
            SPI_Driver::global().RxCallbackHandler();
    }
}

}//namespace connectivity