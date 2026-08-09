#include <array>
#include <cstdint>
#include "main.h"
#include "stm32g4xx_hal.h"
#include "stm32g4xx_hal_gpio.h"
#include "seven_segment_display.hpp"
#include "status.hpp"

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
    /* pins masks for digits on display with common annode */
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
} /* annon namespace*/




namespace display {

Status Display::show_digit_on_display(){

    /* Turn off every digit before changing the segment pattern. */
    HAL_GPIO_WritePin(GPIOC, kAllDigits_Msk, GPIO_PIN_RESET);

    if(state_.digits[state_.activeDigit]!=kDispalyBlankDigit){

    /* write number's segments to GPIOA */
     HAL_GPIO_WritePin(GPIOA, kAllSegments, GPIO_PIN_RESET);
     HAL_GPIO_WritePin(GPIOA, kDigitPatterns[state_.digits[state_.activeDigit]], GPIO_PIN_SET);
    
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

    for(std::size_t i = 0; i < state_.digits.size() -1; ++i){
    if(state_.digits[i] != 0U){
        break;
    }
    else
    {
        state_.digits[i] = kDispalyBlankDigit;
     }
   }

    return Status::kOk;
 }
}/* namespace display*/
