#include "app.h"
#include "main.h"

#include "FreeRTOS.h"
#include "task.h"

namespace
{
class Application
{
public:
    void setup() noexcept
    {
        const BaseType_t task_created = xTaskCreate(
            led_task,
            "LED",
            256,
            nullptr,
            tskIDLE_PRIORITY + 1,
            nullptr);

        if (task_created != pdPASS)
        {
            Error_Handler();
        }

        vTaskStartScheduler();

        /* The scheduler returns only when the idle task cannot be created. */
        Error_Handler();
    }

    void loop() noexcept
    {
    }

private:
    static void led_task(void *argument) noexcept
    {
        (void) argument;

        __HAL_RCC_GPIOA_CLK_ENABLE();

        GPIO_InitTypeDef led_gpio = {};
        led_gpio.Pin = GPIO_PIN_5;
        led_gpio.Mode = GPIO_MODE_OUTPUT_PP;
        led_gpio.Pull = GPIO_NOPULL;
        led_gpio.Speed = GPIO_SPEED_FREQ_LOW;
        HAL_GPIO_Init(GPIOA, &led_gpio);

        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);

        for (;;)
        {
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
            vTaskDelay(pdMS_TO_TICKS(100));
        }
    }
};

Application application;
} // namespace

extern "C" void app_setup(void)
{
    application.setup();
}

extern "C" void app_loop(void)
{
    application.loop();
}

extern "C" void vApplicationMallocFailedHook(void)
{
    taskDISABLE_INTERRUPTS();
    for (;;)
    {
    }
}

extern "C" void vApplicationStackOverflowHook(TaskHandle_t task, char *task_name)
{
    (void) task;
    (void) task_name;

    taskDISABLE_INTERRUPTS();
    for (;;)
    {
    }
}

extern "C" void vAssertCalled(const char *file, int line)
{
    (void) file;
    (void) line;

    taskDISABLE_INTERRUPTS();
    for (;;)
    {
    }
}
