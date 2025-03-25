# platform/CMakeLists.txt

# Add the platform-specific library
add_library(platform
    src/platform.c
    src/platform_config.c
)

# Include the headers for this library
target_include_directories(platform PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/include
)

target_link_libraries(platform PRIVATE
    pico_stdlib
    hardware_i2c
)