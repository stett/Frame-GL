#pragma once
#include <functional>
#include "frame/Component.h"
#include "frame/System.h"
#include "frame/Node.h"
#include "frame_gl/components/Transform.h"
#include "frame_gl/systems/Window.h"
#include "frame_gl/systems/Render.h"
#include "frame_gl/math.h"

namespace frame
{
    FRAME_COMPONENT(ClickableArea, Transform) {

    public:
        enum Event {
            Enter = 0,
            Exit,
            LeftDown,
            LeftUp,
            COUNT
        };

    public:
        ClickableArea(const vec2& area=vec2(1.0f, 1.0f)) : _area(area), _mouse_in(false) {
            for (std::size_t i = 0; i < (std::size_t)Event::COUNT; ++i)
                _callback[i] = [](){};
        }

        ClickableArea(std::function<void()> left_down_callback) : ClickableArea() {
            set_callback(Event::LeftDown, left_down_callback);
        }

        ClickableArea(std::function<void()> left_down_callback, std::function<void()> left_up_callback) : ClickableArea(left_down_callback) {
            set_callback(Event::LeftUp, left_up_callback);
        }

    protected:
        void setup(FrameInterface& frame, Entity* entity) {
            render = frame->systems().get<Render>();
            window = Window::get_window(frame);
            window->mouse_button.listen(this, &ClickableArea::mouse_button_callback);
            window->mouse_position.listen(this, &ClickableArea::mouse_position_callback);
        }

        void teardown(FrameInterface& frame, Entity* entity) {
            window->mouse_button.ignore(this);
            window->mouse_position.ignore(this);
        }

    public:
        const vec2& area() const { return _area; }
        bool mouse_in() const { return _mouse_in; }
        ClickableArea* set_callback(Event event, std::function<void()> callback) {
            _callback[event] = callback;
            return this;
        }

    public:
        void mouse_button_callback(int button, int action, int mods) {
            if (!mouse_in()) return;

            if (action == GLFW_PRESS) {
                _callback[Event::LeftDown]();
            } else if (action == GLFW_RELEASE) {
                _callback[Event::LeftUp]();
            }
        }

        void mouse_position_callback(const vec2& screen_position) {

            // Grab the display camera from the frame's renderer.
            if (!render) return;
            auto camera = render->display_camera();
            if (!camera) return;

            // Project the mouse's screen position onto the XY plane,
            // given the projection and view matrices of the camera.
            vec3 world_position;
            vec2 homogeneous_screen_position(
                2.0f * (float)screen_position.x / (float)window->size().x - 1.0f,
                1.0f - 2.0f * (float)screen_position.y / (float)window->size().y);
            bool on_plane = camera->get_world_point(world_position, homogeneous_screen_position);
            //if (!on_plane)
            //    return;

            // Transform the mouse into local coordinates and see if the mouse falls within our area.
            auto area_half = _area * 0.5f;
            auto local_position = get<Transform>()->world_inverse() * vec4(world_position, 1.0f);
            bool mouse_in_now =
                local_position.x >= -area_half.x &&
                local_position.y >= -area_half.y &&
                local_position.x < area_half.x &&
                local_position.y < area_half.y;

            // If the mouse is entering or leaving the area, call a callback
            if (mouse_in_now && !_mouse_in)         _callback[Event::Enter]();
            else if (!mouse_in_now && _mouse_in)    _callback[Event::Exit]();
            _mouse_in = mouse_in_now;
        }

    private:
        vec2 _area;
        bool _mouse_in;
        std::function<void()> _callback[Event::COUNT];
        Render* render;
        Window* window;
    };
}