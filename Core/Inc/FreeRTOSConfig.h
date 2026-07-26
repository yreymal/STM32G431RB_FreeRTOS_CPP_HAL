#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

extern uint32_t SystemCoreClock;
void vAssertCalled(const char *file, int line);

#ifdef __cplusplus
}
#endif

/* Scheduler */
#define configUSE_PREEMPTION                    1
#define configUSE_TIME_SLICING                   1
#define configUSE_PORT_OPTIMISED_TASK_SELECTION 1
#define configUSE_TICKLESS_IDLE                  0
#define configCPU_CLOCK_HZ                       (SystemCoreClock)
#define configTICK_RATE_HZ                       ((TickType_t) 1000)
#define configTICK_TYPE_WIDTH_IN_BITS            TICK_TYPE_WIDTH_32_BITS
#define configMAX_PRIORITIES                     5
#define configMINIMAL_STACK_SIZE                 ((uint16_t) 128)
#define configMAX_TASK_NAME_LEN                  16
#define configIDLE_SHOULD_YIELD                  1
#define configUSE_IDLE_HOOK                      0
#define configUSE_TICK_HOOK                      0

/* Memory: heap_4.c uses this pool for task stacks and kernel objects. */
#define configSUPPORT_STATIC_ALLOCATION          0
#define configSUPPORT_DYNAMIC_ALLOCATION         1
#define configTOTAL_HEAP_SIZE                    ((size_t) (12U * 1024U))
#define configAPPLICATION_ALLOCATED_HEAP          0
#define configUSE_MALLOC_FAILED_HOOK             1
#define configCHECK_FOR_STACK_OVERFLOW           2
#define configUSE_NEWLIB_REENTRANT               0

/* Synchronisation features kept in the minimal build. */
#define configUSE_MUTEXES                        1
#define configUSE_RECURSIVE_MUTEXES              0
#define configUSE_COUNTING_SEMAPHORES            1
#define configUSE_TASK_NOTIFICATIONS             1
#define configTASK_NOTIFICATION_ARRAY_ENTRIES    1
#define configQUEUE_REGISTRY_SIZE                0
#define configUSE_TIMERS                         0

/* STM32G431 implements four NVIC priority bits. */
#define configPRIO_BITS                          4
#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY  15
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY 5
#define configKERNEL_INTERRUPT_PRIORITY          \
    (configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS))
#define configMAX_SYSCALL_INTERRUPT_PRIORITY     \
    (configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS))

/* Map the FreeRTOS port handlers onto the STM32 startup vector names. */
#define vPortSVCHandler                       SVC_Handler
#define xPortPendSVHandler                    PendSV_Handler
#define configCHECK_HANDLER_INSTALLATION         1

/* API functions used by the application and the shared SysTick handler. */
#define INCLUDE_vTaskDelay                       1
#define INCLUDE_xTaskGetSchedulerState           1

#define configASSERT(expression)                                      \
    do                                                                \
    {                                                                 \
        if ((expression) == 0)                                        \
        {                                                             \
            vAssertCalled(__FILE__, __LINE__);                        \
        }                                                             \
    } while (0)

#endif /* FREERTOS_CONFIG_H */
