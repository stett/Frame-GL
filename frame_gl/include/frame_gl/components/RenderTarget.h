#pragma once
#include <string>
#include "frame/Component.h"
#include "frame/Resource.h"
#include "frame_gl/data/FrameBuffer.h"
#include "frame_gl/math.h"

namespace frame
{
    FRAME_COMPONENT(RenderTarget) {
    public:
        RenderTarget(const ivec2& size=ivec2(300), int display_layer=-1, bool depth=true, const vec4& clear_color=vec4(0.0f)) :
            _display_layer(display_layer), _display_priority(display_layer), buffer(Resource<FrameBuffer>(size, depth, clear_color)) {}

        RenderTarget(const Resource<FrameBuffer>& buffer, int display_layer=-1) :
            _display_layer(display_layer), _display_priority(display_layer), buffer(buffer) {}

        ~RenderTarget() {}

    public:

        void bind_texture(unsigned int texture_unit) { buffer->bind_texture(texture_unit); }
        void bind_target(bool clear=false) { buffer->bind_target(clear); }
        void unbind_texture() { buffer->unbind_texture(); }
        void unbind_target() { buffer->unbind_target(); }

        /*
        void set_display_layer(int display_layer) {
            //
            // TODO: If this is being set as the display target for a layer (ie, display!=-1),
            //       then unset "display" for all other RenderTarget components on this layer somehow.
            //
            _display_layer = display_layer;
        }
        */

        RenderTarget* set_clear_color(const vec4& clear_color) {
            buffer->set_clear_color(clear_color);
            return this;
        }

        RenderTarget* set_size(const ivec2& size) {
            buffer->set_size(size);
            return this;
        }

        RenderTarget* set_display_priority(int display_priority) {
            _display_priority = display_priority;
            return this;
        }

        int display_layer() const { return _display_layer; }
        int display_priority() const { return _display_priority; }
        const ivec2& size() const { return buffer->size(); }
        const vec4& clear_color() const { return buffer->clear_color(); }
        bool depth() const { return buffer->depth(); }

    public:

        static bool compare(const RenderTarget* a, const RenderTarget* b) {
            return a->_display_priority < b->_display_priority;
        }


    private:
        int _display_layer;
        int _display_priority;
        Resource<FrameBuffer> buffer;
    };
}