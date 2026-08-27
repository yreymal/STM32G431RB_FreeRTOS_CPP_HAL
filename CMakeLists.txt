cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME STM32G431RB_FreeRTOS_CPP_HAL)

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(${CMAKE_PROJECT_NAME} LANGUAGES C CXX ASM)

set(PROJECT_INC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Core/Inc")
set(PROJECT_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Core/Src")

set(STM32_STARTUP_FILE
    "${CMAKE_CURRENT_SOURCE_DIR}/startup_stm32g431xx.s"
    CACHE FILEPATH "STM32 startup assembly file")
set(STM32_LINKER_SCRIPT
    "${CMAKE_CURRENT_SOURCE_DIR}/STM32G431xx_FLASH.ld"
    CACHE FILEPATH "STM32G431RB Flash linker script")
set(FREERTOS_KERNEL_DIR
    "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/FreeRTOS-Kernel"
    CACHE PATH "FreeRTOS kernel directory")

set(STM32_REQUIRED_PATHS
    "${STM32_STARTUP_FILE}"
    "${STM32_LINKER_SCRIPT}"
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/stm32cubemx/CMakeLists.txt"
    "${CMAKE_CURRENT_SOURCE_DIR}/Drivers/CMSIS/Device/ST/STM32G4xx/Include"
    "${CMAKE_CURRENT_SOURCE_DIR}/Drivers/CMSIS/Include"
    "${FREERTOS_KERNEL_DIR}/tasks.c"
    "${FREERTOS_KERNEL_DIR}/list.c"
    "${FREERTOS_KERNEL_DIR}/include/FreeRTOS.h"
    "${FREERTOS_KERNEL_DIR}/portable/GCC/ARM_CM4F/port.c")

foreach(REQUIRED_PATH IN LISTS STM32_REQUIRED_PATHS)
    if(NOT EXISTS "${REQUIRED_PATH}")
        message(FATAL_ERROR
            "Required STM32 platform path is missing: ${REQUIRED_PATH}")
    endif()
endforeach()

set(MCU_OPTIONS
    -mcpu=cortex-m4
    -mthumb
    -mfpu=fpv4-sp-d16
    -mfloat-abi=hard)

set(PROJECT_WARNING_OPTIONS
    -Wall
    -Wextra
    -Wpedantic
    -Wconversion
    -Wsign-conversion
    -Wshadow
    -Wundef
    -Wformat=2
    -Wdouble-promotion
    -Werror)

# Terminal colors used by the configuration and post-build summaries.
option(ENABLE_COLORED_OUTPUT "Enable ANSI colors in build output" ON)

if(ENABLE_COLORED_OUTPUT)
    string(ASCII 27 COLOR_ESCAPE)
    set(COLOR_RESET "${COLOR_ESCAPE}[0m")
    set(COLOR_BOLD "${COLOR_ESCAPE}[1m")
    set(COLOR_CYAN "${COLOR_ESCAPE}[36m")
    set(COLOR_GREEN "${COLOR_ESCAPE}[32m")
    set(COLOR_YELLOW "${COLOR_ESCAPE}[33m")
    set(COLOR_MAGENTA "${COLOR_ESCAPE}[35m")
else()
    set(COLOR_RESET "")
    set(COLOR_BOLD "")
    set(COLOR_CYAN "")
    set(COLOR_GREEN "")
    set(COLOR_YELLOW "")
    set(COLOR_MAGENTA "")
endif()

# CMAKE_COLOR_DIAGNOSTICS handles colored diagnostics on CMake 3.24 and newer.
if(CMAKE_VERSION VERSION_LESS "3.24")
    if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
        add_compile_options(
            $<$<COMPILE_LANGUAGE:C,CXX>:-fdiagnostics-color=always>
        )
    elseif(CMAKE_C_COMPILER_ID MATCHES "Clang")
        add_compile_options(
            $<$<COMPILE_LANGUAGE:C,CXX>:-fcolor-diagnostics>
        )
    endif()
endif()

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

target_compile_options(STM32_Drivers PRIVATE
    ${MCU_OPTIONS}
    -ffunction-sections
    -fdata-sections
    -fstack-usage)

# Add FreeRTOS kernel
add_library(freertos_config INTERFACE)
target_include_directories(freertos_config INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/Core/Inc
)

set(FREERTOS_PORT "GCC_ARM_CM4F" CACHE STRING "FreeRTOS port")
set(FREERTOS_HEAP "4" CACHE STRING "FreeRTOS heap implementation")
add_subdirectory("${FREERTOS_KERNEL_DIR}" FreeRTOS-Kernel)

foreach(FREERTOS_TARGET freertos_kernel freertos_kernel_port)
    target_compile_options(${FREERTOS_TARGET} PRIVATE
        ${MCU_OPTIONS}
        -ffunction-sections
        -fdata-sections
        -fstack-usage)
endforeach()

message(STATUS "")
message(STATUS "${COLOR_BOLD}${COLOR_MAGENTA}Firmware build configuration${COLOR_RESET}")
message(STATUS "${COLOR_CYAN}  Project       :${COLOR_RESET} ${COLOR_GREEN}${CMAKE_PROJECT_NAME}${COLOR_RESET}")
message(STATUS "${COLOR_CYAN}  Build type    :${COLOR_RESET} ${COLOR_YELLOW}${CMAKE_BUILD_TYPE}${COLOR_RESET}")
message(STATUS "${COLOR_CYAN}  Generator     :${COLOR_RESET} ${CMAKE_GENERATOR}")
message(STATUS "${COLOR_CYAN}  Target        :${COLOR_RESET} ${COLOR_GREEN}STM32G431RBTx / Cortex-M4F${COLOR_RESET}")
message(STATUS "${COLOR_CYAN}  C compiler    :${COLOR_RESET} ${CMAKE_C_COMPILER} ${COLOR_YELLOW}(${CMAKE_C_COMPILER_ID} ${CMAKE_C_COMPILER_VERSION})${COLOR_RESET}")
message(STATUS "${COLOR_CYAN}  C++ compiler  :${COLOR_RESET} ${CMAKE_CXX_COMPILER} ${COLOR_YELLOW}(${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION})${COLOR_RESET}")
message(STATUS "${COLOR_CYAN}  Standards     :${COLOR_RESET} ${COLOR_GREEN}C${CMAKE_C_STANDARD}, C++${CMAKE_CXX_STANDARD}${COLOR_RESET}")
message(STATUS "${COLOR_CYAN}  CPU flags     :${COLOR_RESET} ${MCU_OPTIONS}")
message(STATUS "${COLOR_CYAN}  FreeRTOS      :${COLOR_RESET} ${COLOR_GREEN}${FREERTOS_PORT}, heap_${FREERTOS_HEAP}${COLOR_RESET}")
message(STATUS "${COLOR_CYAN}  Linker script :${COLOR_RESET} ${CMAKE_SOURCE_DIR}/STM32G431xx_FLASH.ld")
message(STATUS "${COLOR_CYAN}  Output        :${COLOR_RESET} ${CMAKE_BINARY_DIR}")
message(STATUS "")

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    "${PROJECT_SRC_DIR}/app.cpp"
    "${PROJECT_SRC_DIR}/seven_segment_display.cpp"
    "${PROJECT_SRC_DIR}/timerWrapper.cpp"
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined include paths
)

# Keep strict warnings focused on project-owned C++ code. HAL, CMSIS, and
# FreeRTOS are third-party headers and are therefore treated as system headers.
target_include_directories(${CMAKE_PROJECT_NAME} SYSTEM PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32G4xx_HAL_Driver/Inc"
    "${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32G4xx_HAL_Driver/Inc/Legacy"
    "${CMAKE_CURRENT_SOURCE_DIR}/Drivers/CMSIS/Device/ST/STM32G4xx/Include"
    "${CMAKE_CURRENT_SOURCE_DIR}/Drivers/CMSIS/Include"
    "${FREERTOS_KERNEL_DIR}/include"
    "${FREERTOS_KERNEL_DIR}/portable/GCC/ARM_CM4F"
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
)

target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE
    ${MCU_OPTIONS}
    $<$<COMPILE_LANGUAGE:C,CXX>:-ffunction-sections>
    $<$<COMPILE_LANGUAGE:C,CXX>:-fdata-sections>
    $<$<COMPILE_LANGUAGE:C,CXX>:-fstack-usage>
    $<$<COMPILE_LANGUAGE:CXX>:${PROJECT_WARNING_OPTIONS}>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-threadsafe-statics>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-use-cxa-atexit>
)

# Remove wrong libob.a library dependency when using cpp files
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx
    freertos_kernel
    c
    m
    nosys

    # Add user defined libraries
)

target_link_options(${CMAKE_PROJECT_NAME} PRIVATE
    ${MCU_OPTIONS}
    "-T${STM32_LINKER_SCRIPT}"
    -static
    --specs=nano.specs
    --specs=nosys.specs
    -Wl,--gc-sections
    -Wl,--cref
    -Wl,--print-memory-usage
    "-Wl,-Map=${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}.map"
)

set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES
    SUFFIX ".elf"
    LINK_DEPENDS "${STM32_LINKER_SCRIPT}"
)

# Generate flashable images and print a compact size/artifact summary.
set(FIRMWARE_HEX "${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}.hex")
set(FIRMWARE_BIN "${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}.bin")

add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
    BYPRODUCTS
        "${FIRMWARE_HEX}"
        "${FIRMWARE_BIN}"
    COMMAND ${CMAKE_OBJCOPY} -O ihex
        $<TARGET_FILE:${CMAKE_PROJECT_NAME}> "${FIRMWARE_HEX}"
    COMMAND ${CMAKE_OBJCOPY} -O binary
        $<TARGET_FILE:${CMAKE_PROJECT_NAME}> "${FIRMWARE_BIN}"
    COMMAND ${CMAKE_COMMAND}
        "-DENABLE_COLOR=${ENABLE_COLORED_OUTPUT}"
        "-DSIZE_TOOL=${CMAKE_SIZE}"
        "-DELF_FILE=$<TARGET_FILE:${CMAKE_PROJECT_NAME}>"
        "-DHEX_FILE=${FIRMWARE_HEX}"
        "-DBIN_FILE=${FIRMWARE_BIN}"
        -P "${CMAKE_SOURCE_DIR}/cmake/firmware-report.cmake"
    COMMENT "${COLOR_CYAN}Creating HEX/BIN images and reporting firmware size${COLOR_RESET}"
    VERBATIM
)

set_property(TARGET ${CMAKE_PROJECT_NAME} APPEND PROPERTY
    ADDITIONAL_CLEAN_FILES "${FIRMWARE_HEX};${FIRMWARE_BIN}"
)
