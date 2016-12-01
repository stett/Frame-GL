project(FrameGL)

include_directories(${FrameGL_SOURCE_DIR}/include)
include_directories(${FrameGL_SOURCE_DIR}/external/frame/frame/include)
include_directories(${FrameGL_SOURCE_DIR}/external/glew-cmake/include)
include_directories(${FrameGL_SOURCE_DIR}/external/glfw/include)
include_directories(${FrameGL_SOURCE_DIR}/external/gli)
include_directories(${FrameGL_SOURCE_DIR}/external/gli/external/glm)

OPTION( glew-cmake_BUILD_SHARED "Build the shared glew library" OFF )

add_subdirectory(external/frame)
add_subdirectory(external/glew-cmake)
add_subdirectory(external/glfw)

file(GLOB frame_gl_sources "source/*.cpp")

add_library(frame_gl STATIC ${frame_gl_sources})

target_link_libraries(frame_gl libglew_static glfw ${GLFW_LIBRARIES})