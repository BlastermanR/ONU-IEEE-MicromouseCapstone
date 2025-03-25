# interrupts/CMakeLists.txt

# Add the platform-specific library
add_library(state_machine
    src/state_machine.cpp
)

# Include the headers for this library
target_include_directories(state_machine PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/include
)

target_link_libraries(state_machine PRIVATE
    pico_stdlib
)