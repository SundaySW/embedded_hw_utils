#pragma once

#include "ranges"

#include "async_tim_tasks/async_tim_tasks.hpp"

namespace connectivity{

template<typename Port_t, std::size_t pool_size>
struct InterfaceDriver{
    void ProcessTasks(){
        for(auto& port: ports_)
            port.ProcessTask();
    }

    InterfaceDriver(){
        RUN_ASYNC({ self->ProcessTasks();});
    }

    template<typename ...Types>
    void PlaceTask(typename Port_t::Hadle_t handle, Types&& ...args){
        if(auto it = std::ranges::find(ports_, handle, &Port_t::Hadle_t::GetHandle); it != ports_.end())
            it->PlaceTask(std::forward<Types>(args)...);
    }

    template<typename ...Types>
    void PlaceTask(typename Port_t::Hadle_t handle, typename Port_t::Task_t&& task){
        if(auto it = std::ranges::find(ports_, handle, &Port_t::Hadle_t::GetHandle); it != ports_.end())
            it->PlaceTask(std::forward<Port_t::Task_t>(task));
    }

    auto GetPort(typename Port_t::Hadle_t handle){
        if(auto it = std::ranges::find(ports_, handle, &Port_t::GetHandle); it != ports_.end())
            return it;
        else
            return ports_.begin();
    }

    void PlacePort(typename Port_t::Hadle_t handle){
        if(auto it = std::ranges::find(ports_, nullptr, &Port_t::GetHandle); it != ports_.end())
            it->SetHandle(handle);
    }

    void TxHandler(typename Port_t::Hadle_t handle){
        for(std::size_t i = 0; i < pool_size; i++){
            auto& port = ports_[i];
            if(port.GetHandle() == handle){
                port.TxHandler();
                return;
            }
        }
    }

    void RxHandler(typename Port_t::Hadle_t handle){
        if(auto it = std::ranges::find(ports_, handle, &Port_t::GetHandle); it != ports_.end())
            it->RxHandler();
    }

    void ErrorHandler(typename Port_t::Hadle_t handle){
        if(auto it = std::ranges::find(ports_, handle, &Port_t::GetHandle); it != ports_.end())
            it->ErrorHandler();
    }

protected:
    std::array<Port_t, pool_size> ports_;
};

} //namespace connectivity