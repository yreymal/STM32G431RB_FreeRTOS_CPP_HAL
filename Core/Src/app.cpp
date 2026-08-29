/*
 * app.h declares the C-compatible entry points called by main.c.
 * main.h provides STM32 HAL types, GPIO definitions, and Error_Handler().
 */
#include "app.h"
#include "main.h"
#include "seven_segment_display.hpp"
/* FreeRTOS.h contains kernel types/configuration; task.h contains task APIs. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include <cstdint>
#include"timerWrapper.hpp"
#include "tim.h"
class Application{
public:
void start() noexcept{
    display_.start();
    tim4_.startPWM();
    sinPWMCh4_.start();
    
    vTaskStartScheduler();
    Error_Handler();
}

void loop() noexcept{}

private:
display::Display display_;
tim_wr::TimerWrapper tim4_{htim4};
tim_wr::SinPwm sinPWMCh4_{htim4};
};

namespace{
Application app{};
}
extern "C" void app_setup(void) {
    app.start();
}

extern "C" void app_loop(void)
{
    app.loop();
}

/*
 * FreeRTOS calls this hook because configUSE_MALLOC_FAILED_HOOK is 1.
 * Interrupts are disabled and execution stops so a debugger can inspect the
 * failure. A production application could also log or signal the error.
 */
extern "C" void vApplicationMallocFailedHook(void)
{
    taskDISABLE_INTERRUPTS();
    for (;;)
    {
    }
}

/*
 * FreeRTOS calls this hook because configCHECK_FOR_STACK_OVERFLOW is 2.
 * task and task_name identify the damaged task. They are deliberately marked
 * unused for now; inspect them in the debugger when this hook is reached.
 */
extern "C" void vApplicationStackOverflowHook(TaskHandle_t task, char *task_name)
{
    (void) task;
    (void) task_name;

    taskDISABLE_INTERRUPTS();
    for (;;)
    {
    }
}

/*
 * configASSERT() calls this function with the source location of a failed
 * kernel check. Inspect file and line in the debugger to find the cause.
 */
extern "C" void vAssertCalled(const char *file, int line)
{
    (void) file;
    (void) line;

    taskDISABLE_INTERRUPTS();
    for (;;)
    {
    }
}
