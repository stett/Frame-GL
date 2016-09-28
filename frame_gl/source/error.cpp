#include "frame/Log.h"
#include "frame_gl/error.h"

void frame::_gl_check(char* file, int line, char* error) {
    std::string error_string = std::string(error);
    Log::error("OpenGL error (in file " + std::string(file) + " at line " + std::to_string(line) + "): " + error_string);
}
