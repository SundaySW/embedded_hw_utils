#pragma once

#include "i2c_fwd.hpp"

namespace connectivity{

using TaskT = utils::Task<i2c_buffer_size>;

struct I2CTask: TaskT{

    template<typename ... Types>
    I2CTask(Types&&... args)
        :TaskT(std::forward<decltype(args)>(args)...)
    {}

    template<typename ... Types>
    I2CTask(uint16_t addr, Types&&... args)
        :utils::Task<>(std::forward<decltype(args)>(args)...)
        ,addr_(addr)
    {}

    I2CAddrT GetAddr(){
        return addr_;
    }

//        template<typename ...Types>
//        I2CTask(Types&&... args)
//        requires meta::utils::has_type<I2CAddrT, Types...>
//        {
//            meta::utils::remove_arg_by_type_and_invoke<I2CAddrT>(
//                    [&]<typename... Args_t>(Args_t &&...args_){
//                        *this = TaskT(std::forward<Args_t>(args_)...);}
//                    ,std::forward<Types>(args)...);
//            addr_ = meta::utils::magic_get<I2CAddrT>(std::forward<Types>(args)...);
//        };

private:
    I2CAddrT addr_;
};

}//namespace connectivity