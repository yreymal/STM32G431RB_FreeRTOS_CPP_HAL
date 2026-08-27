#include "timerWrapper.hpp"
#include "main.h"
#include "task.h"

namespace tim_wr{
    void TimerWrapper::startPWM(){
        pwmQueue = xQueueCreate(1U, sizeof(std::uint16_t));

        if(pwmQueue==nullptr){
            Error_Handler();
        }

        const BaseType_t display_task_created = xTaskCreate(
            task,
            "PWM",
            256,
            this,
            tskIDLE_PRIORITY + 2,
            nullptr);
            
        if(display_task_created!= pdPASS){
            Error_Handler();
        }
    }

    void TimerWrapper::task(void *argument) noexcept{
        auto* self = static_cast<TimerWrapper*>(argument);

       const std::uint32_t arr = __HAL_TIM_GET_AUTORELOAD(&self->htim_);
       const std::uint16_t step = 100U;
       std::uint16_t pulse  = 0U;

        HAL_TIM_PWM_Start(&self->htim_, TIM_CHANNEL_1);
        __HAL_TIM_SET_COMPARE(&self->htim_, TIM_CHANNEL_1, pulse);
        
        for(;;){
            for(;pulse < arr;pulse+=step){

            __HAL_TIM_SET_COMPARE(&self->htim_, TIM_CHANNEL_1, pulse);
            vTaskDelay(pdMS_TO_TICKS(20));
           }
            if(pulse>=arr){
                     pulse = static_cast<std::uint16_t>(arr);
                }

            for(;pulse >step; pulse-=step){
            __HAL_TIM_SET_COMPARE(&self->htim_, TIM_CHANNEL_1, pulse);
            vTaskDelay(pdMS_TO_TICKS(20));
           }

           pulse = 0U;
        __HAL_TIM_SET_COMPARE(&self->htim_, TIM_CHANNEL_1, pulse);
         vTaskDelay(pdMS_TO_TICKS(20));
        }
    }

}