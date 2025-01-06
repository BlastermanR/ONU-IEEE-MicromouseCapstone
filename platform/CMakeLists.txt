# platform/CMakeLists.txt

# Add the platform-specific library
add_library(Platform
    src/platform.c
)

# Include the headers for this library
target_include_directories(Platform PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/include
)

target_link_libraries(Platform PRIVATE
    pico_stdlib
    hardware_i2c
)