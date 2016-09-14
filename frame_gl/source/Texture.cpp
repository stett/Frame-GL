#define GLEW_STATIC
#include <GL/glew.h>
#include "frame/Log.h"
#include "frame_gl/data/Texture.h"
#include "frame_gl/error.h"
using namespace frame;

Texture::Texture(const ivec2& size, bool multisample) : _size(size), _multisample(multisample) {
    glGenTextures(1, &_id);
    glBindTexture(GL_TEXTURE_2D, _id);
    glTexImage2D(GL_TEXTURE_2D, 0, (int)GL_RGBA32F_ARB, size.x, size.y, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (int)GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (int)GL_CLAMP_TO_EDGE);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (int)GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (int)GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (int)GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (int)GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);

    gl_check();

    Log::success("Texture (" + std::to_string(size.x) + "x" + std::to_string(size.y) + ") created with ID " + std::to_string(_id));
}

Texture::~Texture() { glDeleteTextures(1, &_id); }

void Texture::bind(unsigned int texture_unit) {
    glActiveTexture(GL_TEXTURE0 + texture_unit);
    glBindTexture(GL_TEXTURE_2D, _id);
}

void Texture::unbind() {
    glBindTexture(GL_TEXTURE_2D, 0);
}
