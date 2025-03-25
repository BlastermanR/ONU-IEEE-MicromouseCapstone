# interrupts/CMakeLists.txt

# Add the platform-specific library
add_library(motor_control
    src/motor_control.cpp
)

# Include the headers for this library
target_include_directories(motor_control PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/include
    ${CMAKE_CURRENT_LIST_DIR}/../include
)

target_link_libraries(motor_control PRIVATE
    pico_stdlib
    hardware_pwm
    platform
    shared_data
)