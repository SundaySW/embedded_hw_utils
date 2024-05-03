#pragma once

#include <cstdint>
#include "type_traits"

namespace PIN_BOARD{

    enum LOGIC_LEVEL{
        LOW = 0,
        HIGH = 1,
    };

    enum PullState: uint8_t{
        no_pull,
        pull_up,
        pull_down
    };

    struct PinReadable {};

    struct PinWriteable {};

    struct PinSwitchable {};

    template<typename InterfaceType>
    class PIN{
    public:
        template<typename T = InterfaceType>
        requires(std::is_base_of<PinReadable, T>::value || std::is_base_of<PinSwitchable, T>::value)
        constexpr LOGIC_LEVEL getState(){
            currentState_ = getValue();
            return currentState_;
        }

        template<typename T = InterfaceType>
        requires(std::is_base_of<PinWriteable, T>::value || std::is_base_of<PinSwitchable, T>::value)
        constexpr void setValue(LOGIC_LEVEL value){
            if(inverted_){
                if (value) port_->BRR = (uint32_t)pin_;
                else port_->BSRR = (uint32_t)pin_;
            }else{
                if (value) port_->BSRR = (uint32_t)pin_;
                else port_->BRR = (uint32_t)pin_;
            }
            currentState_ = value;
        }

        template<typename T = InterfaceType>
        requires(std::is_base_of<PinWriteable, T>::value || std::is_base_of<PinSwitchable, T>::value)
        constexpr void togglePinState(){
            uint32_t odr = port_->ODR;
            port_->BSRR = ((odr & pin_) << 16U) | (~odr & pin_);
        }

        constexpr void setInverted() {
            inverted_ = true;
        }

        [[nodiscard]] constexpr inline GPIO_TypeDef* getPort() const{
            return port_;
        }
        [[nodiscard]] constexpr inline uint16_t getPin() const{
            return pin_;
        }

        [[nodiscard]] constexpr LOGIC_LEVEL* GetPinStatePtr(){
            return static_cast<LOGIC_LEVEL*>(&currentState_);
        }

//        constexpr void setAFSpi(uint8_t spi_n = 1){
//            port_->MODER = (port_->MODER)
//                           & (~(GPIO_MODER_MODE0 << (position_ * 2U)))
//                           | ((GPIO_MODE_AF_PP & GPIO_MODE) << (position_ * 2U));
//            port_->AFR[position_ >> 3U] = (port_->AFR[position_ >> 3U])
//                                          & (~(0xFU << ((position_ & 0x07U) * 4U)))
//                                          | ((spi_n) << ((position_ & 0x07U) * 4U));
//        }

        constexpr void setAsOutput(){
            port_->AFR[position_ >> 3u] &= ~(0xFu << ((position_ & 0x07u) * 4u));
            port_->MODER = (port_->MODER & ~(0x03 << (2 * position_))) | (0x01 << (2 * position_));
        }

        constexpr void setAsInput(){
            port_->AFR[position_ >> 3u] &= ~(0xFu << ((position_ & 0x07u) * 4u));
            port_->MODER = port_->MODER & ~(0x03 << (2 * position_));
        }

        constexpr void setPull(PullState pullState){
            port_->PUPDR = port_->PUPDR
                           & (~(GPIO_PUPDR_PUPD0 << (position_ * 2U)))
                           | (pullState << (position_ * 2U));
        }

        constexpr void CalcPosition(){
            for(uint8_t i = 0; i < 32; i++)
                if(pin_ >> i)
                    position_ = i;
        }

        PIN() = default;

        constexpr void Reset(GPIO_TypeDef* incomePortPtr, uint16_t incomePin){
            port_ = incomePortPtr,
            pin_ = incomePin;
            CalcPosition();
        }

        constexpr explicit PIN(GPIO_TypeDef* incomePortPtr, uint16_t incomePin)
                : port_(incomePortPtr),
                  pin_(incomePin)
        {
            CalcPosition();
        };

    protected:
    private:
        LOGIC_LEVEL currentState_ = LOW;
        GPIO_TypeDef* port_{nullptr};
        uint16_t pin_{0};
        uint8_t position_{0};
        bool inverted_ = false;

        template<typename T = InterfaceType>
        requires(std::is_base_of<PinReadable, T>::value || std::is_base_of<PinSwitchable, T>::value)
        constexpr LOGIC_LEVEL getValue(){
            LOGIC_LEVEL retVal;
            if((port_->IDR & pin_) != (uint32_t)LOGIC_LEVEL::LOW)
                retVal = LOGIC_LEVEL::HIGH;
            else
                retVal = LOGIC_LEVEL::LOW;
            if(inverted_)
                return (retVal ? LOGIC_LEVEL::LOW : LOGIC_LEVEL::HIGH);
            else
                return retVal;
        }
    };

} // namespace PIN_BOARD