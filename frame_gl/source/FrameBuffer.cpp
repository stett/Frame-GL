#define GLEW_STATIC
#include <GL/glew.h>
#include "frame/Log.h"
#include "frame_gl/data/FrameBuffer.h"
#include "frame_gl/data/Texture.h"
#include "frame_gl/error.h"
using namespace frame;

FrameBuffer::FrameBuffer(const ivec2& size, bool depth, const vec4& clear_color) : _size(size), _depth(depth), _clear_color(clear_color) {

    // Create the frame buffer object
    glGenFramebuffers(1, &frame_buffer_id);
    glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer_id);

    // Create a render buffer, and attach it to FBO's depth attachment
    if (depth) {
        glGenRenderbuffers(1, &depth_buffer_id);
        glBindRenderbuffer(GL_RENDERBUFFER, depth_buffer_id);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, size.x, size.y);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_buffer_id);
    }

    // Create texture and attach FBO's color 0 attachment
    bool multisample = true;
    texture = new Texture(size, multisample);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture->id(), 0);

    // Check frame buffer status
    int status = (int)glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        Log::error("Failed to create frame buffer: (GL Error ID " + std::to_string(status) + ")");
        return;
    }

    gl_check();

    // Log some success, boyeee
    Log::success("Frame buffer (" + std::to_string(size.x) + "x" + std::to_string(size.y) + ") created with ID " + std::to_string(frame_buffer_id));

    // Unbind the target
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

FrameBuffer::~FrameBuffer() { delete texture; }

void FrameBuffer::bind_target(bool clear) {

    // Bind the frame buffer
    glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer_id);

    // Set the viewport
    glViewport(0, 0, size().x, size().y);

    // Set up OpenGL state
    if (_depth) glEnable(GL_DEPTH_TEST);
    else glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glDisable(GL_STENCIL_TEST);

    // Clear the frame buffer if necessary
    if (clear) {
        glClearColor(_clear_color.r, _clear_color.g, _clear_color.b, _clear_color.a);
        glClear(GL_COLOR_BUFFER_BIT | (_depth ? GL_DEPTH_BUFFER_BIT : 0));
    }

    gl_check();
}

void FrameBuffer::bind_texture(unsigned int texture_unit) {
    texture->bind(texture_unit);
}

void FrameBuffer::unbind_target() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::unbind_texture() {
    texture->unbind();
}
