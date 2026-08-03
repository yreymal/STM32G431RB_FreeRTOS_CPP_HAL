#pragma once

#include <cstdint>
#include <array>
#include "status.hpp"
namespace display{


class Display{
    
static constexpr std::uint16_t kMaxDisplayNumber = 9'999U;
static constexpr std::uint8_t kDispalyBlankDigit = 255;

struct State{
std::uint16_t number{};
std::uint8_t activeDigit{};
std::array<std::uint8_t, 4U>digits{};
};

State state_{};

public:
//Display(const Display&) = delete;
Display& operator=(const Display&) = delete;

Status setNumber(const std::uint16_t number);

/* just 1st step, check on what pointer points to, then if ok,
 * split 4 digit number into 4 sigle digits into state array members */
 Status encodeNumber();

 Status show_digit_on_display();

 Status displayPinsInit();

};
} //namespace display