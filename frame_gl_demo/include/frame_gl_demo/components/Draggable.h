#pragma once
#include "frame/Component.h"
#include "frame/System.h"
#include "frame/Node.h"
#include "frame_gl/components/Transform.h"
#include "frame_gl/components/ClickableArea.h"
#include "frame_gl/systems/Render.h"
#include "frame_gl/math.h"
using namespace frame;

namespace frame_demo
{
    FRAME_COMPONENT(Draggable, Transform, ClickableArea) {
    public:
        Draggable(const vec3& point = vec3(0.0f), const vec3& normal = vec3(0.0f, 0.0f, 1.0f))
        : plane_point(point), plane_normal(normal), dragging(false) {}

    public:
        Draggable* set_plane(const vec3& point, const vec3& normal) {
            plane_point = point;
            plane_normal = normal;
            return this;
        }

    protected:
        void setup(FrameInterface& frame, Entity* entity) {
            render = frame->systems().get<Render>();
            window = Window::get_window(frame);
            window->mouse_position.listen(this, &Draggable::mouse_position_callback);

            get<ClickableArea>()->set_callback(ClickableArea::Event::LeftDown, [this](){ this->dragging = true; });
            get<ClickableArea>()->set_callback(ClickableArea::Event::LeftUp,   [this](){ this->dragging = false; });
            get<ClickableArea>()->set_callback(ClickableArea::Event::Exit,     [this](){ this->dragging = false; });
        }

        void teardown(FrameInterface& frame, Entity* entity) {
            //
            // TODO: Clear the callbacks?
            //
        }

    public:
        void mouse_position_callback(const vec2& screen_position) {

            // Grab the display camera from the frame's renderer.
            auto camera = render->display_camera();
            if (!camera) return;

            // Project the mouse's screen position onto the plane,
            // given the projection and view matrices of the camera.
            vec3 world_position;
            vec2 homogeneous_screen_position(
                2.0f * (float)screen_position.x / (float)window->size().x - 1.0f,
                1.0f - 2.0f * (float)screen_position.y / (float)window->size().y);
            bool on_plane = camera->get_world_point(world_position, homogeneous_screen_position, plane_point, plane_normal);

            // If we're dragging, set our world position to the mouse's position
            /*
            TODO: REENABLE!
            if (dragging)
                get<Transform>()->set_world_translation(world_position);
            */
        }

    private:
        vec3 plane_point;
        vec3 plane_normal;
        bool dragging;
        Render* render;
        Window* window;
    };
}