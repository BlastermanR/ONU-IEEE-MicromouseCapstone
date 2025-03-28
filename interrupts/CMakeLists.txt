# interrupts/CMakeLists.txt

# Add the platform-specific library
add_library(interrupts
    src/interrupts.cpp
    src/quadrature_encoder.cpp
)

# Generate PIO header
pico_generate_pio_header(interrupts ${CMAKE_CURRENT_LIST_DIR}/src/quadrature_encoder.pio)

# Include the headers for this library
target_include_directories(interrupts PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/include
)

target_link_libraries(interrupts PRIVATE
    platform
    pico_stdlib
    hardware_i2c
    hardware_pwm
    hardware_pio
)