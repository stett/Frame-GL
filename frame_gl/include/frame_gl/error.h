#pragma once
#define GLEW_STATIC
#include <string>
#include <GL/glew.h>
#include "frame/Log.h"

#if FRAME_LOG
#define gl_check() {            \
    GLenum err = glGetError();  \
    if (err != GL_NO_ERROR) {   \
        std::string error_string = std::string((char*)glewGetErrorString(err)); \
        Log::error("OpenGL error (in file " + std::string(__FILE__) + " at line " + std::to_string(__LINE__) + "): " + error_string); \
    }                           \
}
#else
#define gl_check()
#endif
