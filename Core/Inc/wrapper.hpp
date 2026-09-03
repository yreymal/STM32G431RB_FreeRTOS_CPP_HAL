#pragma once
#include "main.h"
#include <cstdint>
#include <array>
#include "status.hpp"
#include "FreeRTOS.h"
#include "queue.h"
#include "seven_segment_display.hpp"
namespace wr{

class PwmWrapper{

    public:
    void startPWM();
    explicit PwmWrapper(TIM_HandleTypeDef &htim)noexcept:htim_{htim}{};
    PwmWrapper(const PwmWrapper&) = delete;
    PwmWrapper& operator=(const PwmWrapper &) = delete;

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

namespace adc{
    class ADC{
        public:
       explicit ADC(ADC_HandleTypeDef* hadc, display::Display& display)noexcept:hadc_{hadc},display_{display}{};
        void startADC();
        void startAdcDMA();
        void stopAdcDMA();
        void stopADC();
        void onADCFinished(ADC_HandleTypeDef *hadc);
        std::uint16_t averageDmaBuffer() const noexcept;
        private:
        ADC_HandleTypeDef* hadc_;
        display::Display& display_;
        /* DMA variables */
        static constexpr std::uint8_t DMAsamples_ = 32U;
        std::array<std::uint16_t, DMAsamples_> dmaBuffer_{};
    };

}