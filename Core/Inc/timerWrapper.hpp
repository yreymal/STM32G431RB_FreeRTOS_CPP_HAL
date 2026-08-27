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
    explicit TimerWrapper(TIM_HandleTypeDef &htim)noexcept:htim_{htim}{
        
    }
    TimerWrapper(const TimerWrapper&) = delete;
    TimerWrapper& operator=(const TimerWrapper &) = delete;

    private:
     TIM_HandleTypeDef& htim_;
    static void task(void* argument)noexcept;
    QueueHandle_t pwmQueue{};
    
};

}