#pragma once

#include "embedded_hw_utils/IO/pin.hpp"

#include "spi_fwd.hpp"

namespace connectivity{

using PinT = PIN_BOARD::PIN<PIN_BOARD::PinWriteable>*;
using TaskT = utils::Task<>;

struct SpiTask: TaskT{
    void ChipRelease(){
        if(pin_)
            pin_.value()->setValue(PIN_BOARD::HIGH);
    }
    void ChipSelect(){
        if(pin_)
            pin_.value()->setValue(PIN_BOARD::LOW);
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
    std::optional<PIN_BOARD::PIN<PIN_BOARD::PinWriteable>*> pin_;
};

}//namespace connectivity