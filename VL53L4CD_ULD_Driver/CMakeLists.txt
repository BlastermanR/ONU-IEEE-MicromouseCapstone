# Add the sensor driver library
add_library(VL53L4CD_Driver
    src/sensor_api.c
)

# Include the headers for this library
target_include_directories(VL53L4CD_Driver PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/include
)

target_link_libraries(VL53L4CD_Driver PRIVATE
    Platform
)
