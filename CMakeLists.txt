# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

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

project(ONU_IEEE_Micromouse C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1
add_executable(ONU_IEEE_Micromouse 
    # Add main file
    src/ONU_IEEE_Micromouse.cpp

    tests/sensorTests/basicSensorTest.cpp
)

pico_set_program_name(ONU_IEEE_Micromouse "ONU_IEEE_Micromouse")
pico_set_program_version(ONU_IEEE_Micromouse "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(ONU_IEEE_Micromouse 0)
pico_enable_stdio_usb(ONU_IEEE_Micromouse 1)

# Add the standard library to the build
target_link_libraries(ONU_IEEE_Micromouse
    pico_stdlib
)

# Add the standard include files to the build
target_include_directories(ONU_IEEE_Micromouse PUBLIC
    # Add generic headerfiles
    #${CMAKE_CURRENT_LIST_DIR}/include

    ${CMAKE_CURRENT_LIST_DIR}/tests/sensorTests/
)


# Add subdirectories for platform and sensor driver
add_subdirectory(platform)
add_subdirectory(VL53L4CD_ULD_Driver)


# Add any user requested libraries
target_link_libraries(ONU_IEEE_Micromouse 
    hardware_i2c
    hardware_gpio
    Platform
    VL53L4CD_Driver
)

pico_add_extra_outputs(ONU_IEEE_Micromouse)

