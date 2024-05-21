#pragma once

#include "pin.hpp"

struct InputSignal{
    enum PinConnectionState{
        connected,
        new_connection,
        last_disconnected,
        no_device
    };

    explicit InputSignal(pin_board::PIN<pin_board::Readable> pin, uint32_t debounce_time) noexcept
        :pin_(pin)
        ,debounce_time_(debounce_time)
    {}

//    constexpr explicit InputSignal(pin_board::PIN<pin_board::Readable> pin, uint32_t debounce_time) noexcept
//        : pin_(pin)
//        , debounce_time_(debounce_time)
//    {}

    PinConnectionState GetPinConnectionState(){
        auto retV = no_device;
        if(signal_state_ && last_pin_connection_state_)
            retV = connected;
        else if(signal_state_ && !last_pin_connection_state_)
            retV = new_connection;
        else if(!signal_state_ && last_pin_connection_state_)
            retV = last_disconnected;
        last_pin_connection_state_ = signal_state_;
        return retV;
    }

    constexpr void UpdatePin() {
        if(pin_.getState()){
            if(active_time_ < debounce_time_)
                active_time_++;
        }else
            active_time_ = 0;

        if(active_time_ >= debounce_time_)
            SetState(pin_board::HIGH);
        else
            SetState(pin_board::LOW);
    }

    constexpr void SetDebounceTime(uint32_t time){
        debounce_time_ = time;
    }

    constexpr void InvertSignalPin(){
        pin_.setInverted();
    }

    [[nodiscard]] constexpr pin_board::logic_level getState() const {
        return signal_state_;
    }
private:
    pin_board::PIN<pin_board::Readable> pin_;
    pin_board::logic_level signal_state_ {pin_board::LOW};
    uint32_t debounce_time_;
    uint32_t active_time_{0};
    bool last_pin_connection_state_ {false};

    constexpr void SetState(pin_board::logic_level level){
        signal_state_ = level;
    }
};