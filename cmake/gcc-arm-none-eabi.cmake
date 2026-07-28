# CMake reads this file before project(). Keep compiler discovery here and
# target-specific CPU/runtime/link options in the top-level CMakeLists.txt.
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
list(APPEND CMAKE_TRY_COMPILE_PLATFORM_VARIABLES ARM_GNU_TOOLCHAIN_ROOT)

set(ARM_GNU_TOOLCHAIN_ROOT "" CACHE PATH
    "Arm GNU Toolchain installation root containing the arm-none-eabi directory")

if(ARM_GNU_TOOLCHAIN_ROOT)
    set(ARM_GNU_TOOLCHAIN_BIN "${ARM_GNU_TOOLCHAIN_ROOT}/bin")
    set(ARM_GCC "${ARM_GNU_TOOLCHAIN_BIN}/arm-none-eabi-gcc")
    set(ARM_GXX "${ARM_GNU_TOOLCHAIN_BIN}/arm-none-eabi-g++")
    set(ARM_OBJCOPY "${ARM_GNU_TOOLCHAIN_BIN}/arm-none-eabi-objcopy")
    set(ARM_SIZE "${ARM_GNU_TOOLCHAIN_BIN}/arm-none-eabi-size")

    foreach(ARM_TOOL IN ITEMS ARM_GCC ARM_GXX ARM_OBJCOPY ARM_SIZE)
        if(NOT EXISTS "${${ARM_TOOL}}")
            message(FATAL_ERROR
                "Required Arm GNU tool is missing: ${${ARM_TOOL}}")
        endif()
    endforeach()
else()
    find_program(ARM_GCC arm-none-eabi-gcc REQUIRED)
    find_program(ARM_GXX arm-none-eabi-g++ REQUIRED)
    find_program(ARM_OBJCOPY arm-none-eabi-objcopy REQUIRED)
    find_program(ARM_SIZE arm-none-eabi-size REQUIRED)
endif()

execute_process(
    COMMAND "${ARM_GCC}" -dumpfullversion
    OUTPUT_VARIABLE ARM_GCC_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE
    COMMAND_ERROR_IS_FATAL ANY)

if(ARM_GCC_VERSION VERSION_LESS "11")
    message(FATAL_ERROR
        "Arm GNU Toolchain 11 or newer is required; found ${ARM_GCC_VERSION}")
endif()

message(STATUS "Arm GNU Toolchain: ${ARM_GCC_VERSION} (${ARM_GNU_TOOLCHAIN_ROOT})")

set(CMAKE_C_COMPILER "${ARM_GCC}" CACHE FILEPATH "Arm C compiler" FORCE)
set(CMAKE_CXX_COMPILER "${ARM_GXX}" CACHE FILEPATH "Arm C++ compiler" FORCE)
set(CMAKE_ASM_COMPILER "${ARM_GCC}" CACHE FILEPATH "Arm assembler driver" FORCE)
set(CMAKE_OBJCOPY "${ARM_OBJCOPY}" CACHE FILEPATH "Arm objcopy utility" FORCE)
set(CMAKE_SIZE "${ARM_SIZE}" CACHE FILEPATH "Arm size utility" FORCE)
