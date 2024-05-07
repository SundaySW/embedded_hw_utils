#pragma once

#include "pin.hpp"

struct Button{
    constexpr explicit Button(GPIO_TypeDef* port, uint16_t pin) noexcept
        : pin_(port, pin)
    {
//        pin_.setInverted();
    }

    pin_board::PIN<pin_board::Readable> pin_;

    constexpr uint16_t operator()() const{
        return pin_.getPin();
    }
    constexpr bool operator==(uint16_t otherPin) const{
        return pin_.getPin() == otherPin;
    }
    constexpr pin_board::logic_level getState(){
        return pin_.getState();
    }
};
