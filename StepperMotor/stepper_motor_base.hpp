#pragma once

#include <cstdlib>
#include <algorithm>

#include "embedded_hw_utils/IO/pin.hpp"
#include "stm32g4xx_hal_tim.h"

namespace Motor{
    enum class Direction{
        BACKWARDS = 0,
        FORWARD = 1
    };

    enum Mode
    {
        IDLE,
        ACCEL,
        CONST,
        DECCEL,
        in_ERROR
    };

    enum MOTOR_EVENT {
        EVENT_NULL = 0,
        EVENT_STOP,
        EVENT_CSS,  //	constant speed reached
        EVENT_CSE   //  constant speed end
    };
}

namespace StepperMotor{
    struct StepperCfg
    {
        pin_board::PIN<pin_board::Writeable> step_pin;
        pin_board::PIN<pin_board::Writeable> direction_pin;
        pin_board::PIN<pin_board::Writeable> enable_pin;
        TIM_HandleTypeDef* htim {};
        uint32_t timChannel {0x00000001U};
        uint32_t criticalNofSteps {};
        bool directionInverted {false};
    };

    using namespace Motor;
    class StepperMotorBase{
    public:
        using MOTOR_IOS = pin_board::PIN<pin_board::Writeable>;

        const StepperMotorBase& operator=(const StepperMotorBase &) = delete;
        StepperMotorBase& operator=(StepperMotorBase &) = delete;
        StepperMotorBase(StepperMotorBase&) = delete;
        StepperMotorBase(StepperMotorBase&&)= delete;

        void MotorRefresh(){
            if(mode_ == IDLE || mode_ == Mode::in_ERROR)
                return;
            AppCorrection();
            CalcSpeed_();
            CalcRegValue_();
        }

        virtual void AppCorrection() = 0;
        virtual void AccelerationImpl() = 0;

        void MakeMotorTask(uint32_t start_speed,
                           uint32_t max_speed,
                           Direction dir = currentDirection_,
                           uint32_t steps = kCriticalNofSteps_)
        {
            if(motorMoving_)
                StopMotor();
            Vmin_ = start_speed;
            Vmax_ = max_speed;
            V_ = start_speed;
            SetDirection_(dir);
            StartMotor_(steps);
        }

        void StopMotor(){
            if(motorMoving_){
                HAL_TIM_PWM_Stop_IT(htim_, timChannel_);
//                enable_pin_.setValue(pin_board::HIGH);
                step_pin_.setValue(pin_board::LOW);
                motorMoving_ = false;
                mode_ = Mode::IDLE;
                event_ = EVENT_STOP;
            }
        }

        void ChangeDirection(){
            SetDirection_(static_cast<bool>(currentDirection_) ? Direction::BACKWARDS : Direction::FORWARD);
            V_ = Vmin_;
            mode_ = Mode::ACCEL;
            CalcRegValue_();
            task_step_ = 0;
            accel_step_ = 0;
            uSec_accel_ = 0;
        }

        void ChangeDirectionAndGo(uint32_t steps){
            SetDirection_(currentDirection_ == Motor::Direction::FORWARD ? Direction::BACKWARDS : Direction::FORWARD);
            steps_to_go_ = steps;
            V_ = Vmin_;
            CalcRegValue_();
            task_step_ = 0;
            accel_step_ = 0;
            uSec_accel_ = 0;
            mode_ = Mode::ACCEL;
        }

        [[nodiscard]] bool IsMotorMoving() const {
            return motorMoving_;
        }

        [[nodiscard]] Mode GetMode() const {
            return mode_;
        }

        [[nodiscard]] MOTOR_EVENT GetEvent() const {
            return event_;
        }

        [[nodiscard]] Direction GetCurrentDirection() const {
            return currentDirection_;
        }

    protected:
        StepperMotorBase() = delete;
        explicit StepperMotorBase(StepperMotor::StepperCfg& cfg)
                :step_pin_(cfg.step_pin),
                 direction_pin_(cfg.direction_pin),
                 enable_pin_(cfg.enable_pin),
                 timChannel_(cfg.timChannel),
                 htim_(cfg.htim),
                 directionInverted_(cfg.directionInverted)
        {
            kCriticalNofSteps_ = cfg.criticalNofSteps;
            timer_tick_Hz_ = SystemCoreClock / (cfg.htim->Instance->PSC);
        };

        MOTOR_IOS step_pin_;
        MOTOR_IOS direction_pin_;
        MOTOR_IOS enable_pin_;

        TIM_HandleTypeDef* htim_;
        uint32_t timChannel_;
        uint32_t timer_tick_Hz_;

        inline static uint32_t kCriticalNofSteps_ {0};
        int steps_to_go_ {0};
        int task_step_ {0};
        int accel_step_ {0};

        uint32_t V_ {0};
        uint32_t Vmin_ {0};
        uint32_t Vmax_ {0};

        uint32_t uSec_accel_ {0};

        inline static Direction currentDirection_ {Direction::FORWARD};
        Mode mode_ {Mode::IDLE};
        MOTOR_EVENT event_ {EVENT_STOP};

        bool directionInverted_ {false};
        bool motorMoving_ {false};

        void StartMotor_(uint32_t steps){
            if(!motorMoving_){
                accel_step_ = 0;
                uSec_accel_ = 0;
                task_step_ = 0;
                mode_ = Mode::ACCEL;
                motorMoving_ = true;
                steps_to_go_ = steps;
                enable_pin_.setValue(pin_board::LOW);
                CalcRegValue_();
                HAL_TIM_PWM_Start_IT(htim_, timChannel_);
            }
        }

        void CalcRegValue_(){
            if(V_ > 0){
                uint32_t buf = timer_tick_Hz_ / V_;
                if(buf > 0 && buf < UINT16_MAX){
                    __HAL_TIM_SET_AUTORELOAD(htim_, buf);
                    __HAL_TIM_SET_COMPARE(htim_, timChannel_,buf/2);
                }
            }
        }

        void SetDirection_(Direction newDirection){
            currentDirection_ = newDirection;
            if(directionInverted_) direction_pin_.setValue(
                        static_cast<bool>(currentDirection_) ?
                        pin_board::logic_level(Direction::BACKWARDS) : pin_board::logic_level(Direction::FORWARD));
            else direction_pin_.setValue(pin_board::logic_level(currentDirection_));
        }

        void CalcSpeed_(){
            if (mode_ == Mode::IDLE) return;
            switch (mode_)
            {
                case Mode::ACCEL:
                {
                    if (V_ >= Vmax_)
                    {
                        V_ = Vmax_;
                        event_ = EVENT_CSS;
                        mode_ = Mode::CONST;
                    }else
                        AccelerationImpl();

                    if (accel_step_ >= steps_to_go_ / 2)
                    {
                        mode_ = Mode::DECCEL;
                        break;
                    }
                    accel_step_++;
                    uSec_accel_ += timer_tick_Hz_ / V_;
                }
                    break;

                case Mode::CONST:
                {
                    if(task_step_ + accel_step_ >= steps_to_go_){
                        event_ = EVENT_CSE;
                        mode_ = Mode::DECCEL;
                    }
                }
                    break;

                case Mode::DECCEL:
                {
                    if(accel_step_ > 0){
                        AccelerationImpl();
                        if (V_ < Vmin_) V_ = Vmin_;
                        accel_step_--;
                        uSec_accel_ -= timer_tick_Hz_ / V_;
                    }
                }
                    break;

                default:
                    break;
            }

            if (mode_ == Mode::ACCEL || mode_ == Mode::CONST || mode_ == Mode::DECCEL)
                task_step_++;
        }
    };

} //namespace StepperMotor