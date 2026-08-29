#pragma once
#include "main.h"
#include <cstdint>
#include "status.hpp"
#include "FreeRTOS.h"
#include "queue.h"

namespace tim_wr{

class TimerWrapper{

    public:
    void startPWM();
    explicit TimerWrapper(TIM_HandleTypeDef &htim)noexcept:htim_{htim}{};
    TimerWrapper(const TimerWrapper&) = delete;
    TimerWrapper& operator=(const TimerWrapper &) = delete;

    private:
     TIM_HandleTypeDef& htim_;
    static void PWMtask(void* argument)noexcept;
    QueueHandle_t pwmQueue{};
    
};

class SinPwm{
    public:
    explicit SinPwm(TIM_HandleTypeDef& htim):htim_{htim}{};
    SinPwm (const SinPwm&) = delete;
    SinPwm& operator=(const SinPwm&) = delete;
    void start();
    void onPwmPulseFinished(TIM_HandleTypeDef* htim)noexcept;

    private:
    void updateSinForm();
    TIM_HandleTypeDef& htim_;
    std::uint8_t steps{0};
    float angel{0.0f};
    std::uint16_t pulse_per_angel{0};

};
}

