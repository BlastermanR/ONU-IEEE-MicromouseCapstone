# interrupts/CMakeLists.txt

# Add the platform-specific library
add_library(shared_data
    src/shared_data.cpp
)

# Include the headers for this library
target_include_directories(shared_data PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/include
)

target_link_libraries(shared_data PRIVATE
    pico_stdlib
)