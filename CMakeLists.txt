# ONU IEEE Micromouse Code
# == INITIALIZATION ===================================================================

cmake_minimum_required(VERSION 3.20)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# VSCode Pico Extension Compatability.
# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.0.0)
set(toolchainVersion RISCV_RPI_2_0_0_5)
set(picotoolVersion 2.0.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico2 CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

# == PROJECT DEFINITION ==============================================================
project(ONU_IEEE_Micromouse C CXX ASM)
# DEFINE BUILD OPTIONS
option(ENABLE_LOGGING "Enable logging in the project" OFF)
option(ENABLE_DEBUG "Use fast math optimizations" OFF)
option(RUN_TESTS "Enable logging in the project" OFF)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable.
add_executable(ONU_IEEE_Micromouse 
    src/ONU_IEEE_Micromouse.cpp
    src/control_process.cpp
    src/gpio.cpp
    src/robot.cpp
)

pico_set_program_name(ONU_IEEE_Micromouse "ONU_IEEE_Micromouse")
pico_set_program_version(ONU_IEEE_Micromouse "0.1")

# Disable conflicting functionality
# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(ONU_IEEE_Micromouse 0)

# Build for Debug
if (ENABLE_DEBUG)
    pico_enable_stdio_usb(ONU_IEEE_Micromouse 1) # Enable USB Log if DEBUG Enabled
else()
    pico_enable_stdio_usb(ONU_IEEE_Micromouse 0)
endif()

# Add the standard library to the build
target_link_libraries(ONU_IEEE_Micromouse
    pico_stdlib
)

# Add the standard include files to the build
target_include_directories(ONU_IEEE_Micromouse PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/include
)

# Add necessary subdirectories (Libraries)
add_subdirectory(platform)
add_subdirectory(VL53L4CD_ULD_Driver)
add_subdirectory(interrupts)
add_subdirectory(shared_data)
add_subdirectory(state_machine)
add_subdirectory(motor_control)
add_subdirectory(maze)

# Add requested libraries
target_link_libraries(ONU_IEEE_Micromouse 
    hardware_i2c
    hardware_irq
    hardware_timer
    hardware_gpio
    hardware_pwm
    hardware_pio
    pico_multicore 
    pico_util
    platform
    interrupts
    VL53L4CD_Driver
    shared_data
    state_machine
    motor_control
    maze
)

pico_add_extra_outputs(ONU_IEEE_Micromouse)

