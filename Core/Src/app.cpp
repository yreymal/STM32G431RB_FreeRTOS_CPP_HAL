/*
 * app.h declares the C-compatible entry points called by main.c.
 * main.h provides STM32 HAL types, GPIO definitions, and Error_Handler().
 */
#include "app.h"
#include "main.h"

/* FreeRTOS.h contains kernel types/configuration; task.h contains task APIs. */
#include "FreeRTOS.h"
#include "task.h"

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
        /*
         * xTaskCreate() arguments, in order:
         *
         * 1. led_task:
         *      Function that the new task will execute.
         *
         * 2. "LED":
         *      Human-readable task name used by debuggers.
         *
         * 3. 256:
         *      Stack depth in 32-bit words, not bytes. On this MCU that is
         *      256 * 4 = 1024 bytes, allocated from the FreeRTOS heap.
         *
         * 4. nullptr:
         *      Optional value passed to led_task(void *). No value is needed.
         *
         * 5. tskIDLE_PRIORITY + 1:
         *      Priority 1, one level above the idle task at priority 0.
         *
         * 6. nullptr:
         *      Optional place to receive a TaskHandle_t. We do not need to
         *      control this task from elsewhere, so no handle is stored.
         */
        const BaseType_t task_created = xTaskCreate(
            led_task,
            "LED",
            256,
            nullptr,
            tskIDLE_PRIORITY + 1,
            nullptr);

        /* pdPASS means the task and its stack were allocated successfully. */
        if (task_created != pdPASS)
        {
            /* Most likely the FreeRTOS heap did not contain enough free RAM. */
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
    /*
     * A FreeRTOS task function must have the signature void function(void *).
     * It is static because FreeRTOS cannot supply a C++ object ("this" pointer)
     * when it calls the function.
     */
    static void led_task(void *argument) noexcept
    {
        /* This task was created with nullptr, so no argument needs processing. */
        (void) argument;

        /*
         * Peripheral registers cannot be used until their bus clock is on.
         * Enabling an already-enabled GPIOA clock is safe.
         */
        __HAL_RCC_GPIOA_CLK_ENABLE();

        /*
         * Configure PA5, which is connected to the Nucleo board's user LED:
         *   - output push-pull: actively drives the pin low or high;
         *   - no pull resistor: the output driver already controls the level;
         *   - low speed: sufficient for a slowly blinking LED.
         */
        GPIO_InitTypeDef led_gpio = {};
        led_gpio.Pin = GPIO_PIN_5;
        led_gpio.Mode = GPIO_MODE_OUTPUT_PP;
        led_gpio.Pull = GPIO_NOPULL;
        led_gpio.Speed = GPIO_SPEED_FREQ_LOW;
        HAL_GPIO_Init(GPIOA, &led_gpio);

        /* Start from a known state: PA5 low means the user LED is off. */
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);

        /*
         * A task normally contains an infinite loop. It returns control to the
         * scheduler whenever it blocks, waits, or explicitly yields.
         */
        for (;;)
        {
            /* Change PA5 from low to high or from high to low. */
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);

            /*
             * Convert 1000 milliseconds to RTOS ticks and block this task.
             * This is not a busy wait: while the LED task sleeps, FreeRTOS can
             * run other ready tasks or the idle task.
             */
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
