#pragma once
#define GLEW_STATIC
#include <string>
#include <GL/glew.h>
#include "frame/Log.h"

namespace frame
{
    void _gl_check(char* file, int line, char* error);
    /*{
        std::string error_string = std::string(error);
        Log::error("OpenGL error (in file " + std::string(file) + " at line " + std::to_string(line) + "): " + error_string);
    }*/
}

#if FRAME_LOG
#define gl_check() {                            \
    GLenum err = glGetError();                  \
    if (err != GL_NO_ERROR) {                   \
        frame::_gl_check(                       \
            __FILE__, __LINE__,                 \
            (char*)glewGetErrorString(err));    \
    }                                           \
}
#else
#define gl_check()
#endif
