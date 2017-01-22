#define GLEW_STATIC
#include <cstdio>
#include <GL/glew.h>
#include <gli/texture2d.hpp>
#include <gli/convert.hpp>
#include <gli/generate_mipmaps.hpp>
#include <gli/load.hpp>
#include <gli/gl.hpp>
//#include <gli/save.hpp>
#include "frame/Log.h"
#include "frame_gl/data/Texture.h"
#include "frame_gl/error.h"
using namespace frame;

Texture::Texture(const ivec2& size, bool multisample) : _size(size), _multisample(multisample) {
    glGenTextures(1, &_id);
    /*if (_multisample) {
        _target = GL_TEXTURE_2D_MULTISAMPLE;
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, _id);
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, (int)GL_RGBA32F_ARB, size.x, size.y, GL_TRUE);
        glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAX_LEVEL, 0);
        glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_S, (int)GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_T, (int)GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, (int)GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, (int)GL_NEAREST);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
    } else {*/

    _target = GL_TEXTURE_2D;
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

    //}

    gl_check();

    Log::success("Texture (" + std::to_string(size.x) + "x" + std::to_string(size.y) + ") created with ID " + std::to_string(_id));
}

Texture::Texture(const std::string& filename) {

    /*
    if(std::strstr(filename.c_str(), ".dds") > 0 || std::strstr(filename.c_str(), ".ktx") > 0)
    {
        Log::error("Textures must be either .dds or .ktx format!");
        return;
    }
    */

    gli::texture2d texture(gli::load(filename));
    gli::texture2d_array tex(texture);

    _target = GL_TEXTURE_2D;
    _size = texture.extent();

    auto format = gli::gl(gli::gl::profile::PROFILE_GL33).translate(texture.format(), gli::swizzles(GL_BLUE, GL_GREEN, GL_RED, GL_ONE));

    glGenTextures(1, &_id);
    glBindTexture(GL_TEXTURE_2D, _id);
    glTexImage2D(GL_TEXTURE_2D, 0, format.Internal, _size.x, _size.y, 0, format.External, format.Type, texture.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (int)GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (int)GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (int)GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (int)GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (int)GL_NEAREST);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (int)GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);

    gl_check();

    Log::success("Texture (" + std::to_string(_size.x) + "x" + std::to_string(_size.y) + ") created with ID " + std::to_string(_id));
}

Texture::~Texture() { glDeleteTextures(1, &_id); }

void Texture::bind(unsigned int texture_unit) {
    glActiveTexture(GL_TEXTURE0 + texture_unit);
    glBindTexture(_target, _id);
}

void Texture::unbind() {
    glBindTexture(_target, 0);
}

Resource<Texture> Texture::white_pixel() {
    static Resource<Texture> texture(ivec2(1));
    return texture;
}
