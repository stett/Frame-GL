#pragma once
#include "frame/Component.h"
#include "frame/System.h"
#include "frame_gl/components/Transform.h"

namespace frame
{
    FRAME_COMPONENT(TransformDraggable, Transform) {};

    FRAME_SYSTEM(TransformDrag, Node<Transform, TransformDraggable>) {
    public:
        TransformDrag() : selected(nullptr) {}

    protected:
        void step(float dt) {

            auto input = system<Input>();
            auto window = system<Window>();
            auto camera = system<Render>()->display_camera();
            auto debug_draw = system<DebugDraw>();

            bool found = false;
            Transform* hovered = nullptr;
            for (auto e : node<Transform, TransformDraggable>()) {
                Transform* transform = e.get<Transform>();

                if (debug_draw)
                    debug_draw->circle(transform->translation(), 10.0f, 15.0f);

                // Mark the currently selected transform as found...
                // We need to track this in case the object gets deleted.
                if (selected == transform) {
                    found = true;
                }

                if ((selected == nullptr && hovered == nullptr) || (selected == transform)) {

                    // Project the mouse position onto the plane of the bone point normal to the camera
                    vec3 plane_point = transform->translation();
                    vec3 plane_normal = normalize(camera->position() - transform->translation());
                    vec3 mouse_point;
                    bool on_plane = camera->get_world_point(mouse_point,
                        vec2(2.0f * (float)input->mouse_position().x / (float)window->size().x - 1.0f,
                             1.0f - 2.0f * (float)input->mouse_position().y / (float)window->size().y),
                        plane_point, plane_normal);

                    // If a transform is selected, move it to mouse position
                    if (selected == transform) {
                        transform->set_translation(mouse_point);

                        // If we have a debug module, draw the focus point
                        if (debug_draw) {
                            vec3 pos = transform->translation();
                            debug_draw->line(vec3(pos.x, pos.y, 0.0f), pos, frame_gl::axis_color_z);
                            debug_draw->line(vec3(pos.x, 0.0f, pos.z), pos, frame_gl::axis_color_y);
                            debug_draw->line(vec3(0.0f, pos.y, pos.z), pos, frame_gl::axis_color_x);

                            if (abs(pos.z) > 0.00001f)
                                debug_draw->world_text(vec3(pos.x, pos.y, 0.0f), std::to_string(pos.z), frame_gl::axis_color_z);
                            if (abs(pos.y) > 0.00001f)
                                debug_draw->world_text(vec3(pos.x, 0.0f, pos.z), std::to_string(pos.y), frame_gl::axis_color_y);
                            if (abs(pos.x) > 0.00001f)
                                debug_draw->world_text(vec3(0.0f, pos.y, pos.z), std::to_string(pos.x), frame_gl::axis_color_x);
                        }

                    // If the mouse is close to the screen position of the transform, mark it hovered
                    } else if (length(transform->translation() - mouse_point) < 0.5f) {
                        hovered = transform;
                    }
                }
            }

            if (selected && !found)
                selected = nullptr;

            if (selected) {
                if (debug_draw)
                    debug_draw->circle(selected->translation(), 0.0f, 20.0f);
                if (input->mouse_up(Input::MouseButton::Left))
                    selected = nullptr;

            } else if (hovered) {
                if (debug_draw)
                    debug_draw->circle(hovered->translation(), 10.0f, 20.0f);
                if (input->mouse_clicked(Input::MouseButton::Left))
                    selected = hovered;
            }
        }

    private:
        Transform* selected;
    };
}