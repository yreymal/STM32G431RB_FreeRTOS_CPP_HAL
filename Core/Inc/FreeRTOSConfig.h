#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

/*
 * FreeRTOSConfig.h belongs to the application, not to the FreeRTOS kernel.
 *
 * FreeRTOS includes this file while it is being compiled. The macros below
 * select which kernel features are available and describe this MCU to the
 * portable Cortex-M4F layer. Changing a value here changes the kernel build.
 */

#include <stddef.h>
#include <stdint.h>

/*
 * This header is included by C kernel files and may also be included by C++.
 * C linkage prevents the C++ compiler from changing the names of these two
 * functions/variables ("name mangling"), so they match their C definitions.
 */
#ifdef __cplusplus
extern "C" {
#endif

/* Updated by the STM32 system code after SystemClock_Config() runs. */
extern uint32_t SystemCoreClock;

/* Our application-side handler for a failed configASSERT(). */
void vAssertCalled(const char *file, int line);

#ifdef __cplusplus
}
#endif

/* ------------------------------------------------------------------------- */
/* Scheduler                                                                 */
/* ------------------------------------------------------------------------- */

/* 1: a ready higher-priority task immediately preempts a lower-priority one. */
/* It enables preemptive multitasking.
FreeRTOS switches CPU time between multiple tasks, creating the appearance that they run simultaneously on the single-core STM32.
LED task        runs → delays ─────────→ runs
Button task     waiting ─→ runs → waits
Idle task       ───── runs ───────── runs ─────*/
#define configUSE_PREEMPTION                    1

/* 1: ready tasks at the same priority share CPU time on successive ticks. */
/* Suppose two tasks both have priority 1:
LED task:    priority 1
Button task: priority 1
When both are ready, FreeRTOS shares CPU time between them: */
#define configUSE_TIME_SLICING                   1

/*
 * 1: use the Cortex-M count-leading-zeros instruction to find the next task.
 * This is faster than searching every priority in software.
 */
#define configUSE_PORT_OPTIMISED_TASK_SELECTION 1

/* 0: keep the 1 ms tick running even when only the idle task can execute. */
#define configUSE_TICKLESS_IDLE                  0

/* The kernel uses the actual STM32 core-clock value configured at startup. */
#define configCPU_CLOCK_HZ                       (SystemCoreClock)

/* 1000 Hz means one RTOS tick every 1 ms. */
#define configTICK_RATE_HZ                       ((TickType_t) 1000)

/* A 32-bit tick counter wraps after about 49.7 days at 1000 Hz. */
#define configTICK_TYPE_WIDTH_IN_BITS            TICK_TYPE_WIDTH_32_BITS

/*
 * Available priorities are 0 through 4. Priority 0 is the lowest and is used
 * by the idle task. A larger number means a higher task priority.
 */
#define configMAX_PRIORITIES                     5

/*
 * Stack depths in FreeRTOS are measured in StackType_t elements, not bytes.
 * On this 32-bit Cortex-M, 128 words = 512 bytes. This is the idle-task stack.
 */
#define configMINIMAL_STACK_SIZE                 ((uint16_t) 128)

/* Maximum task-name storage, including the terminating '\0' character. */
#define configMAX_TASK_NAME_LEN                  16

/* Let another ready priority-0 task run when the idle task is executing. */
#define configIDLE_SHOULD_YIELD                  1

/* 0: vApplicationIdleHook() is not required or called. */
#define configUSE_IDLE_HOOK                      0

/* 0: vApplicationTickHook() is not required or called every tick. */
#define configUSE_TICK_HOOK                      0

/* ------------------------------------------------------------------------- */
/* Memory                                                                    */
/* ------------------------------------------------------------------------- */

/* 0: tasks and kernel objects are not created with the static-allocation API. */
#define configSUPPORT_STATIC_ALLOCATION          0

/* 1: APIs such as xTaskCreate() may allocate memory from the RTOS heap. */
#define configSUPPORT_DYNAMIC_ALLOCATION         1

/*
 * heap_4.c reserves this complete 12 KiB byte array in RAM at link time.
 * Task stacks and task-control blocks are allocated from this pool at runtime.
 * heap_4 can merge adjacent freed blocks to reduce fragmentation.
 */
#define configTOTAL_HEAP_SIZE                    ((size_t) (12U * 1024U))

/* 0: heap_4.c defines its own ucHeap[] array; the application does not. */
#define configAPPLICATION_ALLOCATED_HEAP          0

/* 1: call vApplicationMallocFailedHook() when an RTOS allocation fails. */
#define configUSE_MALLOC_FAILED_HOOK             1

/*
 * 2: check both ends of a task stack during context switches. If corruption
 * is detected, vApplicationStackOverflowHook() is called.
 */
#define configCHECK_FOR_STACK_OVERFLOW           2

/*
 * 0: do not allocate a separate newlib C-library reentrancy structure for
 * every task. This saves RAM; avoid non-reentrant C-library calls from tasks.
 */
#define configUSE_NEWLIB_REENTRANT               0

/* ------------------------------------------------------------------------- */
/* Synchronisation features                                                  */
/* ------------------------------------------------------------------------- */

/* 1: enable normal mutexes and priority inheritance. */
#define configUSE_MUTEXES                        1

/* 0: omit recursive mutex support because this application does not use it. */
#define configUSE_RECURSIVE_MUTEXES              0

/* 1: allow counting semaphores in addition to binary semaphores. */
#define configUSE_COUNTING_SEMAPHORES            1

/* 1: enable lightweight direct-to-task notifications. */
#define configUSE_TASK_NOTIFICATIONS             1

/* Each task has one notification value/index. */
#define configTASK_NOTIFICATION_ARRAY_ENTRIES    1

/* 0: do not reserve a queue registry used mainly for debugger naming. */
#define configQUEUE_REGISTRY_SIZE                0

/*
 * 0: disable software timers. No timer-service task or timer queue is created,
 * which saves RAM when the application only uses vTaskDelay().
 */
#define configUSE_TIMERS                         0

/* ------------------------------------------------------------------------- */
/* Cortex-M interrupt priorities                                             */
/* ------------------------------------------------------------------------- */

/*
 * STM32G431 implements four NVIC priority bits, giving values 0 through 15.
 * Important: a numerically smaller interrupt value has greater urgency.
 */
#define configPRIO_BITS                          4

/* 15 is the lowest-urgency interrupt priority supported by this MCU. */
#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY  15

/*
 * Interrupts at priorities 5 through 15 may call FreeRTOS "...FromISR" APIs.
 * Higher-urgency interrupts 0 through 4 must never call FreeRTOS APIs.
 */
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY 5

/* Convert the human-readable value 15 to the position used by NVIC registers. */
#define configKERNEL_INTERRUPT_PRIORITY          \
    (configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS))

/* Convert the "...FromISR API allowed" boundary to NVIC register format. */
#define configMAX_SYSCALL_INTERRUPT_PRIORITY     \
    (configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS))

/* ------------------------------------------------------------------------- */
/* Cortex-M exception-handler names                                          */
/* ------------------------------------------------------------------------- */

/*
 * The startup vector table expects SVC_Handler and PendSV_Handler. These
 * aliases make those vector names resolve to the handlers supplied by the
 * FreeRTOS Cortex-M4F port.
 */
#define vPortSVCHandler                       SVC_Handler
#define xPortPendSVHandler                    PendSV_Handler

/* Ask the port to verify that the vector table contains its RTOS handlers. */
#define configCHECK_HANDLER_INSTALLATION         1

/* ------------------------------------------------------------------------- */
/* Optional API functions                                                    */
/* ------------------------------------------------------------------------- */

/* 1: compile vTaskDelay(), which the LED task uses between toggles. */
#define INCLUDE_vTaskDelay                       1

/*
 * 1: compile xTaskGetSchedulerState(). The shared STM32 SysTick handler uses
 * it to determine whether the FreeRTOS scheduler has started.
 */
#define INCLUDE_xTaskGetSchedulerState           1

/*
 * configASSERT is FreeRTOS's internal consistency check. On failure it passes
 * the source filename and line to our hook. The hook disables interrupts and
 * stops, allowing a debugger to inspect the failure.
 */
#define configASSERT(expression)                                      \
    do                                                                \
    {                                                                 \
        if ((expression) == 0)                                        \
        {                                                             \
            vAssertCalled(__FILE__, __LINE__);                        \
        }                                                             \
    } while (0)

#endif /* FREERTOS_CONFIG_H */
