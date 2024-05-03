#pragma once

//#include "embedded_hw_utils/meta/meta_utils.hpp"
#include "uart_fwd.hpp"

namespace connectivity{
    using TaskT = utils::Task<k_buffer_size>;
struct UartTask: TaskT{
    template<typename ... Types>
    UartTask(Types&&... args)
        :TaskT(std::forward<decltype(args)>(args)...)
    {}
//    template<typename ...Types>
//    UartTask(Types&&... args)
//    requires meta::utils::has_type<PinT, Types...>
//    {
//        meta::utils::remove_arg_by_type_and_invoke<PinT>(
//                [&]<typename... Args_t>(Args_t &&...args_){
//                    *this = TaskT(std::forward<Args_t>(args_)...);}
//                ,std::forward<Types>(args)...);
//        pin_ = meta::utils::magic_get<PinT>(std::forward<Types>(args)...);
//    }
};

}//namespace connectivity