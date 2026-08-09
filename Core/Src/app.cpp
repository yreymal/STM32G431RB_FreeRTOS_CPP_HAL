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


/*
 * Names inside an anonymous namespace are visible only in this source file.
 * This keeps Application and the global application object private.
 */
namespace
{
class Application
{
public:
    /*
     * Create all application tasks, then give control to the FreeRTOS
     * scheduler. noexcept documents that exceptions cannot leave this method;
     * the embedded build also compiles C++ with exceptions disabled.
     */
    void setup() noexcept
    {
    
        display_value_queue_ =  xQueueCreate(1U,sizeof(std::uint16_t));

        if(display_value_queue_ == nullptr){
            Error_Handler();
        }

        const BaseType_t display_task_created = xTaskCreate(
            display_task,
            "DISPLAY",
            256,
            nullptr,
            tskIDLE_PRIORITY + 2,
            nullptr);
        /* pdPASS means the task and its stack were allocated successfully. */
        if (display_task_created != pdPASS)
        {
            /* Most likely the FreeRTOS heap did not contain enough free RAM. */
            Error_Handler();
        }
        
        const BaseType_t increaseNumber = xTaskCreate(
            numberIncrease,
            "NUMBER",
            256,
            nullptr,
            tskIDLE_PRIORITY + 1,
            nullptr
        );

        if (increaseNumber != pdPASS)
        {
            Error_Handler();
        }

        /*
         * Start scheduling the ready tasks. After this call, FreeRTOS chooses
         * which task runs and performs context switches on every RTOS tick.
         * A successfully started scheduler never returns to this function.
         */
        vTaskStartScheduler();

        /*
         * Reaching here means FreeRTOS could not create an internal task,
         * usually because the heap was too small.
         */
        Error_Handler();
    }

    /*
     * main.c contains a generated while(1) loop that calls app_loop(). In a
     * FreeRTOS application that loop is not used because setup() starts a
     * scheduler that does not return. Real application work belongs in tasks.
     */
    void loop() noexcept
    {
    }

private:

    inline static QueueHandle_t display_value_queue_{};
    
    /*
     * A FreeRTOS task function must have the signature void function(void *).
     * It is static because FreeRTOS cannot supply a C++ object ("this" pointer)
     * when it calls the function.
     */
    static void display_task(void *argument) noexcept{
        (void) argument;
        display::Display seven_segment{};
        
        std::uint16_t value{};

        for(;;){
            if(xQueueReceive(
                display_value_queue_,&value, 0U) == pdPASS){
                    (void)seven_segment.setNumber(value);
                    (void)seven_segment.encodeNumber();
                
                }
                    (void)seven_segment.show_digit_on_display();
                    vTaskDelay(pdMS_TO_TICKS(1));
            
        }
    }
    static void numberIncrease(void *argument) noexcept{
        (void)argument;
        std::uint16_t number = 1234U;
        
        for(;;){
            (void)xQueueOverwrite(display_value_queue_, &number);
            //number = static_cast<std::uint16_t>(
            //    (number + 1U) % 10'000U);
            ++number;
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
    }

};

/*
 * One statically allocated C++ application object. It is constructed before
 * main() and does not consume memory from the FreeRTOS heap.
 */
Application application;
} // namespace

/*
 * These wrappers have C linkage so main.c can call into the private C++ object.
 * They are the boundary between CubeMX-generated C and application-owned C++.
 */
extern "C" void app_setup(void)
{
    application.setup();
}

extern "C" void app_loop(void)
{
    application.loop();
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
