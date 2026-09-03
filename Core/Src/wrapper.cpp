#include "wrapper.hpp"
#include "main.h"
#include "task.h"
#include <cmath>
#include "stm32g4xx_it.h"


namespace{
    wr::SinPwm* tim4Ch4SinPwm{nullptr};
    adc::ADC* adc_Ch1{nullptr};
}

namespace wr{
    void PwmWrapper::startPWM(){
        pwmQueue = xQueueCreate(1U, sizeof(std::uint16_t));

        if(pwmQueue==nullptr){
            Error_Handler();
        }

        const BaseType_t display_PWMtask_created = xTaskCreate(
            PWMtask,
            "PWM",
            256,
            this,
            tskIDLE_PRIORITY + 2,
            nullptr);
            
        if(display_PWMtask_created!= pdPASS){
            Error_Handler();
        }
    }



    void PwmWrapper::PWMtask(void *argument) noexcept{
        auto* self = static_cast<PwmWrapper*>(argument);

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



    void SinPwm::start(void){
            
    tim4Ch4SinPwm = this;
       
       if(HAL_TIM_PWM_Start_IT(&this->htim_, TIM_CHANNEL_4)!=HAL_OK){
        Error_Handler();
       }

    }




    void SinPwm::onPwmPulseFinished(TIM_HandleTypeDef* htim)noexcept{
        if(htim!=&htim_ || htim->Channel!=HAL_TIM_ACTIVE_CHANNEL_4){
            return;
        }
        updateSinForm();
    }





    void SinPwm::updateSinForm(){
         
        steps++;
      if(steps>=200){
        steps = 0;
      }
     angel = (360.0F / 200.0F) * static_cast<float>(this->steps);

     const std::uint32_t arr = __HAL_TIM_GET_AUTORELOAD(&htim_);

    const float duty =
    0.5F + 0.45F *
    std::sin(angel * 3.14159265F / 180.0F);

    pulse_per_angel = static_cast<std::uint16_t>(
    std::lround(duty * static_cast<float>(arr)));

      __HAL_TIM_SET_COMPARE(&htim_, TIM_CHANNEL_4, pulse_per_angel);
    }

}

extern "C" void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim){
        if(tim4Ch4SinPwm!=nullptr){
            tim4Ch4SinPwm->onPwmPulseFinished(htim);
        }
  }

  namespace adc{
    void ADC::startADC(){
        adc_Ch1 = this;
       if( HAL_ADC_Start_IT(hadc_)!=HAL_OK){
        Error_Handler();
       }

    }

    void ADC::onADCFinished(ADC_HandleTypeDef *hadc){
        if(hadc!=hadc_){
            return;
        }
       
       // const std::uint16_t adcValue = static_cast<std::uint16_t>(HAL_ADC_GetValue(hadc));
    const std::uint16_t adcValue = averageDmaBuffer();

     //  const std::uint16_t voltageInCentivolts = static_cast<std::uint16_t>(
       //  std::round((static_cast<float>(adcValue) * 330.0F))/ 4095.0F);
    // const std::uint16_t voltageInCentivolts = static_cast<std::uint16_t>(
    //        ((static_cast<std::uint32_t>(adcValue) * 50000U) + 2047U) / 4095U);
        BaseType_t taskWoken = pdFALSE;
    //    std::uint16_t temperature = voltageInCentivolts * 100;
    //    display_.writeToDisplay(temperature, 2U, &taskWoken);
    constexpr std::uint32_t kAdcReferenceMv = 330U;
    constexpr std::uint32_t kAdcFullScale = 4095U;

    const std::uint16_t voltageInCentivolts = static_cast<std::uint16_t>(
            ((static_cast<std::uint32_t>(adcValue) * kAdcReferenceMv) + 2047U) / kAdcFullScale);

    display_.writeToDisplay(voltageInCentivolts, 2U, &taskWoken);
       portYIELD_FROM_ISR(taskWoken);
    }

    void ADC::stopADC(){
        if(HAL_ADC_Stop_IT(hadc_)!=HAL_OK){
            Error_Handler();
        }
    }   

    void ADC::startAdcDMA(){
        adc_Ch1 = this;
        if(HAL_ADC_Start_DMA(hadc_, reinterpret_cast<uint32_t*>(dmaBuffer_.data()),
        static_cast<uint32_t>(dmaBuffer_.size()))!=HAL_OK){
            Error_Handler();
        }
    }

    std::uint16_t ADC::averageDmaBuffer()const noexcept{
        std::uint32_t tempSum = 0;
        for(std::size_t i = 0; i <dmaBuffer_.size(); ++i){
            const std::uint16_t sample = dmaBuffer_[i];
            tempSum+=sample;
        }

        std::uint32_t averageValue = ((tempSum + DMAsamples_/2U)/DMAsamples_);
        return static_cast<std::uint16_t>(averageValue);
    }

    void ADC::stopAdcDMA(){
        if(HAL_ADC_Stop_DMA(hadc_)!=HAL_OK){
            Error_Handler();
        }
    }

  

  extern "C" void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc){
    if(adc_Ch1!=nullptr){
        adc_Ch1->onADCFinished(hadc);
    }
   
  }
}
