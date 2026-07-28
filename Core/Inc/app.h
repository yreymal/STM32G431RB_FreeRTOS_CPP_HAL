#ifndef APP_H
#define APP_H

/*
 * This is intentionally app.h rather than app.hpp because both languages use
 * it: main.c is C, while app.cpp is C++. The declarations below form a small
 * bridge that lets the generated STM32 C entry point call our C++ application.
 */

/*
 * C++ normally changes function names to encode type information. extern "C"
 * disables that name mangling so app.cpp exports the exact names app_setup and
 * app_loop expected by main.c. A C compiler does not understand extern "C",
 * therefore the __cplusplus guard hides it while main.c is compiled.
 */
#ifdef __cplusplus
extern "C" {
#endif

/*
 * Creates the application's FreeRTOS task and starts the scheduler.
 * On success the scheduler runs forever, so this function does not return.
 */
void app_setup(void);

/*
 * Compatibility hook for the generated while(1) loop in main.c. It is empty
 * because application work runs in FreeRTOS tasks. It is not normally reached
 * after app_setup() successfully starts the scheduler.
 */
void app_loop(void);

#ifdef __cplusplus
}
#endif

#endif /* APP_H */
