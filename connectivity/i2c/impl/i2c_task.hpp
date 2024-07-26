#pragma once

#include "embedded_hw_utils/connectivity/i2c/i2c_driver_settings.hpp"
//#include "embedded_hw_utils/meta/meta_utils.hpp"

namespace connectivity::i2c{

using InterfaceTask = InterfaceTask<buffer_size>;

struct Task final: InterfaceTask{
    template<typename ... Types>
    Task(Types&&... args)
        :InterfaceTask(std::forward<decltype(args)>(args)...)
    {}

    template<typename ... Types>
    Task(uint16_t addr, Types&&... args)
        :InterfaceTask(std::forward<decltype(args)>(args)...)
        ,addr_(addr)
    {}

    AddrT GetAddr(){
        return addr_;
    }

//        template<typename ...Types>
//        Task(Types&&... args)
//        requires meta::utils::has_type<AddrT, Types...>
//        {
//            meta::utils::remove_arg_by_type_and_invoke<AddrT>(
//                    [&]<typename... Args_t>(Args_t &&...args_){
//                        *this = TaskT(std::forward<Args_t>(args_)...);}
//                    ,std::forward<Types>(args)...);
//            addr_ = meta::utils::magic_get<AddrT>(std::forward<Types>(args)...);
//        };

private:
    AddrT addr_;
};

}//namespace connectivity