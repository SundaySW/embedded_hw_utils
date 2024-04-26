#pragma once

#include <utility>
#include <optional>

#include "embedded_hw_utils/utils/task_queue.hpp"
#include "async_tasks/async_tim_tasks.hpp"
#include "embedded_hw_utils/utils/task.hpp"
#include "spi_task.hpp"

#define SPI_DRIVER_ SPI_Driver::global()
#define SPI_CLEAR_Q_() SPI_Driver::global().ClearQueue()
#define task_q_size (20)

namespace connectivity{

using namespace utils;
struct SPI_Driver{

    explicit SPI_Driver(SPI_HandleTypeDef* hspi)
    {
        spi_handler_ = hspi;
        PLACE_ASYNC_QUICKEST([&]{
            ProcessTask();
        });
    }

    static void SetHandler(SPI_HandleTypeDef* spiHandle){
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

    template<typename ...Types>
    void PlaceTask(Types&& ...args){
        tasks_.push(SpiTask(std::forward<Types>(args)...));
    }

    auto operator()(){
        return spi_handler_;
    }
private:
    bool in_process_{false};
    SpiTask current_task_;
    TaskQueue<SpiTask, task_q_size> tasks_;
    static inline SPI_HandleTypeDef* spi_handler_;
};

extern "C"
{
    void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi){
        if(hspi == SPI_Driver::global()())
            SPI_Driver::global().TxCallbackHandler();
    }

    void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi){
        if(hspi == SPI_Driver::global()())
            SPI_Driver::global().RxCallbackHandler();
    }
}

}//namespace connectivity