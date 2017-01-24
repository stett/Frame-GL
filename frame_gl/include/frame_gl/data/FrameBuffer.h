#pragma once
#include <memory>
#include "frame_gl/math.h"
#include "frame_gl/data/Texture.h"

namespace frame {
    class FrameBuffer {
    public:
        FrameBuffer(const ivec2& size, bool depth=true, const vec4& clear_color=vec4(0.0f), bool multisample=false);
        ~FrameBuffer();
        FrameBuffer(const FrameBuffer& frame_buffer) = delete;
        FrameBuffer& operator=(const FrameBuffer& frame_buffer) = delete;

    public:
        void set_size(const ivec2& size);
        void set_clear_color(const vec3& clear_color) { _clear_color = vec4(clear_color, 1.0f); }
        void set_clear_color(const vec4& clear_color) { _clear_color = clear_color; }
        void bind_target(bool clear=false);
        void bind_texture(unsigned int texture_unit);
        void unbind_target();
        void unbind_texture();
        const ivec2& size() const { return _size; }
        bool depth() const { return _depth; }
        const vec4& clear_color() const { return _clear_color; }

    private:
        ivec2 _size;
        bool _multisample;
        bool _depth;
        vec4 _clear_color;
        Texture* texture;
        unsigned int frame_buffer_id;
        unsigned int depth_buffer_id;
    };
}