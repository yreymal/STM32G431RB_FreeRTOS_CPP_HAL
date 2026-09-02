#include <array>
#include <cstdint>
#include "main.h"
#include "stm32g4xx_hal.h"
#include "stm32g4xx_hal_gpio.h"
#include "seven_segment_display.hpp"
#include "status.hpp"
#include "task.h"

namespace{
    // Section 1: logical segments mapped to physical GPIOA output bits. The names
    // use GPIO masks rather than plain pin numbers so they can be ORed directly.
    constexpr std::uint16_t kSegmentA = A_Pin;
    constexpr std::uint16_t kSegmentB = B_Pin;
    constexpr std::uint16_t kSegmentC = C_Pin;
    constexpr std::uint16_t kSegmentD = D_Pin;
    constexpr std::uint16_t kSegmentE = E_Pin;
    constexpr std::uint16_t kSegmentF = F_Pin;
    constexpr std::uint16_t kSegmentG = G_Pin;
    constexpr std::uint16_t kAllSegments = A_Pin|B_Pin|C_Pin|D_Pin|E_Pin|F_Pin|G_Pin;
    /* Pin masks for digits on a common-anode display. */
    constexpr std::uint16_t kDigit1_Msk = (1UL<<2UL);
    constexpr std::uint16_t kDigit2_Msk = (1UL<<3UL);
    constexpr std::uint16_t kDigit3_Msk = (1UL<<10UL);
    constexpr std::uint16_t kDigit4_Msk = (1UL<<12UL);
    constexpr std::array<std::uint16_t, 4U> kDigitsMask = {kDigit1_Msk, kDigit2_Msk, kDigit3_Msk, kDigit4_Msk};
    constexpr std::uint16_t kAllDigits_Msk =  kDigit1_Msk|kDigit2_Msk|kDigit3_Msk|kDigit4_Msk;

    // Section 2: lookup table for common-anode polarity, where HIGH means OFF.
    // Each entry stores the segments that must remain high/off. For example, digit
    // 0 leaves only G high; digit 8 stores zero because no segment is off.
   constexpr std::array<std::uint16_t, 10U> kDigitPatterns{
        kSegmentG,
    kAllSegments & ~(kSegmentB | kSegmentC),
    kSegmentF | kSegmentC,
    kSegmentE | kSegmentF,
    kSegmentA | kSegmentD | kSegmentE,
    kSegmentB | kSegmentE,
    kSegmentB,
    kAllSegments & ~(kSegmentA | kSegmentB | kSegmentC),
    0U,
    kSegmentE,
   };
} /* anonymous namespace */




namespace display {

    void Display::start(){
        if(!isStarted_){
        display_value_queue_ = xQueueCreate(1U, sizeof(DisplayUpdate));

        if(display_value_queue_ == nullptr){
            Error_Handler();
        }

        const BaseType_t display_task_created = xTaskCreate(
            task,
            "DISPLAY",
            256,
            this,
            tskIDLE_PRIORITY + 2,
            nullptr);
            
        if(display_task_created!= pdPASS){
            Error_Handler();
        }
        isStarted_ = true;
    }
    }

Status Display::show_digit_on_display(){

    /* Turn off every digit before changing the segment pattern. */
    HAL_GPIO_WritePin(GPIOC, kAllDigits_Msk, GPIO_PIN_RESET);

    const auto digit = state_.digits[state_.activeDigit];
    const std::size_t decimalPointDigitIndex =
        state_.digits.size() - state_.decimalPlaces - 1U;
    const bool showDecimalPoint = state_.decimalPlaces != 0U &&
        state_.activeDigit == decimalPointDigitIndex;

    /* The decimal-point cathode has the same active-low polarity as A-G. */
    HAL_GPIO_WritePin(
        Point_GPIO_Port,
        Point_Pin,
        showDecimalPoint ? GPIO_PIN_RESET : GPIO_PIN_SET);

    if(digit != kDisplayBlankDigit){

    /* write number's segments to GPIOA */
     HAL_GPIO_WritePin(GPIOA, kAllSegments, GPIO_PIN_RESET);
     HAL_GPIO_WritePin(GPIOA, kDigitPatterns[digit], GPIO_PIN_SET);
    
     /* light a display's digit */
     HAL_GPIO_WritePin(GPIOC, kDigitsMask[state_.activeDigit], GPIO_PIN_SET);
        
    }
    ++state_.activeDigit;
    if(state_.activeDigit >state_.digits.size() - 1U){
        state_.activeDigit = 0;
    }

    return Status::kOk;

}

Status Display::setNumber(const std::uint16_t number){
    if(number<=kMaxDisplayNumber){
            this->state_.number = number;
    }
    else{
         this->state_.number = kMaxDisplayNumber;
    }
    
    return Status::kOk;
    
}


Status Display::encodeNumber(){
   
   if(state_.number > kMaxDisplayNumber){
        state_.number = kMaxDisplayNumber;
   }

    state_.digits[0] = static_cast<std::uint8_t>(state_.number / 1000U);
    state_.digits[1] = static_cast<std::uint8_t>((state_.number / 100U) % 10U);
    state_.digits[2]= static_cast<std::uint8_t>((state_.number / 10U)  % 10U);
    state_.digits[3] = static_cast<std::uint8_t>(state_.number % 10U);

    const std::size_t decimalPointDigitIndex =
        state_.digits.size() - state_.decimalPlaces - 1U;

    for(std::size_t i = 0; i < state_.digits.size() -1; ++i){
    /* Preserve the leading zero that carries the point: 0.42, not .42. */
    if(state_.decimalPlaces != 0U && i == decimalPointDigitIndex){
        break;
    }
    if(state_.digits[i] != 0U){
        break;
    }
    else
    {
        state_.digits[i] = kDisplayBlankDigit;
     }
   }

    return Status::kOk;
 }

    /*
     * A FreeRTOS task function must have the signature void function(void *).
     * It is static because FreeRTOS cannot supply a C++ object ("this" pointer)
     * when it calls the function.
     */
   void Display::task(void *argument) noexcept{
        auto* self = static_cast<Display*>(argument);
        DisplayUpdate update{};

        for(;;){
            if(xQueueReceive(
                self->display_value_queue_,&update, 0U) == pdPASS){
                    (void)self->setNumber(update.number);
                    self->state_.decimalPlaces =
                        update.decimalPlaces <= kMaxDecimalPlaces ?
                        update.decimalPlaces : kMaxDecimalPlaces;
                    (void)self->encodeNumber();

                }
                    (void)self->show_digit_on_display();
                    vTaskDelay(pdMS_TO_TICKS(1));

        }
    }

    BaseType_t Display::writeToDisplay(std::uint16_t value, BaseType_t* higherPriorityTaskWoken)noexcept{

        return writeToDisplay(value, 0U, higherPriorityTaskWoken);
    }

    BaseType_t Display::writeToDisplay(
        std::uint16_t value,
        std::uint8_t decimalPlaces,
        BaseType_t* higherPriorityTaskWoken) noexcept{

        const DisplayUpdate update{value, decimalPlaces};
        return xQueueOverwriteFromISR(
            display_value_queue_, &update, higherPriorityTaskWoken);
    }
}/* namespace display*/
