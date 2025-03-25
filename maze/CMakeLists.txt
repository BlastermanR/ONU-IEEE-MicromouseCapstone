# platform/CMakeLists.txt

# Add the platform-specific library
add_library(maze
    src/maze.cpp
)

# Include the headers for this library
target_include_directories(maze PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/include
)

# uncomment if needed
#target_link_libraries(maze PRIVATE)