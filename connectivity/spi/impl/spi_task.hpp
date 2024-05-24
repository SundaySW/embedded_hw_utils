#pragma once

#include "embedded_hw_utils/IO/pin.hpp"
#include "embedded_hw_utils/meta/meta_utils.hpp"

#include "spi_fwd.hpp"

namespace connectivity::spi{

using TaskT = Task<spi_buffer_size>;

using PinT = pin_board::PIN<pin_board::Writeable>*;

struct SpiTask: TaskT{

    void ChipRelease(){
        if(pin_)
            pin_.value()->setValue(pin_board::HIGH);
    }
    void ChipSelect(){
        if(pin_)
            pin_.value()->setValue(pin_board::LOW);
    }

    template<typename ... Types>
    SpiTask(Types&&... args)
        :TaskT(std::forward<decltype(args)>(args)...)
    {}

    template<typename ... Types>
    SpiTask(PinT pin, Types&&... args)
        :TaskT(std::forward<decltype(args)>(args)...)
        ,pin_(pin)
    {}

//    template<typename ...Types>
//    SpiTask(Types&&... args)
//    requires meta::utils::has_type<PinT, Types...>
//    {
//        meta::utils::remove_arg_by_type_and_invoke<PinT>(
//                [&]<typename... Args_t>(Args_t &&...args_){
//                    *this = TaskT(std::forward<Args_t>(args_)...);}
//                ,std::forward<Types>(args)...);
//        pin_ = meta::utils::magic_get<PinT>(std::forward<Types>(args)...);
//    };

private:
    std::optional<pin_board::PIN<pin_board::Writeable>*> pin_;
};

}//namespace connectivity