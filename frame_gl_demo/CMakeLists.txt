
# Project name
project(FrameGLDemo)

# Include everything
include_directories(${Frame_SOURCE_DIR}/include)
include_directories(${FrameGL_SOURCE_DIR}/include)
include_directories(${FrameGLDemo_SOURCE_DIR}/include)

file(GLOB demo_sources "source/*.cpp")
add_executable(frame_gl_demo ${demo_sources})

target_link_libraries(frame_gl_demo frame_gl)

add_custom_command(
    TARGET frame_gl_demo
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/assets"
    "${CMAKE_CURRENT_BINARY_DIR}/assets")