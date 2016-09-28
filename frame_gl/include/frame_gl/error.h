#pragma once
#define GLEW_STATIC
#include <string>
#include <GL/glew.h>
#include "frame/Log.h"

namespace frame
{
    void _gl_check(char* file, int line, const std::string& error);
}

#if FRAME_LOG
#define gl_check() {                            \
    GLenum err = glGetError();                  \
    if (err != GL_NO_ERROR) {                   \
        frame::_gl_check(                       \
            __FILE__, __LINE__,                 \
            std::string((char*)glewGetErrorString(err)));\
    }                                           \
}
#else
#define gl_check()
#endif
