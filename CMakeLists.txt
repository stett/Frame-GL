
# CMake version
cmake_minimum_required (VERSION 3.3.2)

# Set up compiler definitions
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    add_definitions(-std=c++0x -Wall -Wextra)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    add_definitions(-W3)
endif()

# Frame GL
add_subdirectory(frame_gl)

# Unit tests and demos
add_subdirectory(frame_gl_demo)
