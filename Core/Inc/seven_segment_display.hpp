#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include "status.hpp"
#include "FreeRTOS.h"
#include "queue.h"
namespace display{


class Display{

bool isStarted_ = {false};
static constexpr std::uint16_t kMaxDisplayNumber = 9'999U;
static constexpr std::uint8_t kDisplayBlankDigit = 255;
static constexpr std::size_t kDisplayDigitCount = 4U;
static constexpr std::uint8_t kMaxDecimalPlaces =
    static_cast<std::uint8_t>(kDisplayDigitCount - 1U);

struct State{
std::uint16_t number{};
std::uint8_t activeDigit{};
std::uint8_t decimalPlaces{};
std::array<std::uint8_t, kDisplayDigitCount>digits{};
};

struct DisplayUpdate{
std::uint16_t number{};
std::uint8_t decimalPlaces{};
};

State state_{};

Status setNumber(const std::uint16_t number);

/* just 1st step, check on what pointer points to, then if ok,
 * split 4 digit number into 4 single digits into state array members */
 Status encodeNumber();

 Status show_digit_on_display();

 Status displayPinsInit();

 QueueHandle_t display_value_queue_{};

 static void task(void *argument) noexcept;

public:
void start();

Display() = default;
Display& operator=(const Display&) = delete;
Display(const Display&) = delete;
BaseType_t writeToDisplay(std::uint16_t value, BaseType_t* higherPriorityTaskWoken)noexcept;
BaseType_t writeToDisplay(
    std::uint16_t value,
    std::uint8_t decimalPlaces,
    BaseType_t* higherPriorityTaskWoken) noexcept;

};
} //namespace display
