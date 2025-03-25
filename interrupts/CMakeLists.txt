# interrupts/CMakeLists.txt

# Add the platform-specific library
add_library(interrupts
    src/interrupts.cpp
)

# Include the headers for this library
target_include_directories(interrupts PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/include
)

target_link_libraries(interrupts PRIVATE
    platform
    pico_stdlib
    hardware_i2c
    hardware_pwm
)