#pragma once
#include <string>
#include "frame/System.h"
#include "frame_gl/components/Transform.h"
#include "frame_gl/components/Camera.h"
#include "frame_gl/systems/Input.h"
#include "frame_gl/systems/DebugDraw.h"
#include "frame_gl/components/Manipulate3D.h"
#include "frame_gl/math.h"
#include "frame_gl/colors.h"
using namespace frame;

namespace frame
{
    FRAME_SYSTEM(Manipulate3DOperator, Node<Transform, Manipulate3D>) {

    public:
        void step(float dt) {

            auto input = frame()->systems().get<Input>();
            if (input == nullptr) return;

            auto debug_draw = frame()->systems().get<frame_gl::DebugDraw>();

            for (auto manipulator : node<Transform, Manipulate3D>()) {
                auto transform = manipulator.get<Transform>();
                auto manipulate = manipulator.get<Manipulate3D>();

                // Only control this object if the manipulation button is being held
                if (input->mouse_down(manipulate->button())) {

                    // Angle manipulation
                    manipulate->shift_angle(input->mouse_delta(), -10.0f * input->mouse_scroll().y);

                    // Translation manipulation
                    glm::vec3 focus_delta;
                    if (input->key_down(KEY_W)) focus_delta.z -= 1.0f;
                    if (input->key_down(KEY_S)) focus_delta.z += 1.0f;
                    if (input->key_down(KEY_A)) focus_delta.x += 1.0f;
                    if (input->key_down(KEY_D)) focus_delta.x -= 1.0f;
                    if (input->key_down(KEY_E)) focus_delta.y += 1.0f;
                    if (input->key_down(KEY_Q)) focus_delta.y -= 1.0f;
                        manipulate->shift_focus(focus_delta);

                    if (input->key_pressed(KEY_SPACE))
                        manipulate->set_focus(vec3(0.0f));
                }

                // Interpolate motion
                manipulate->update(dt);

                // If we have a debug module, draw the focus point
                if (debug_draw) {
                    vec3 pos = manipulate->focus();
                    debug_draw->line(vec3(pos.x, pos.y, 0.0f), pos, frame_gl::axis_color_z);
                    debug_draw->line(vec3(pos.x, 0.0f, pos.z), pos, frame_gl::axis_color_y);
                    debug_draw->line(vec3(0.0f, pos.y, pos.z), pos, frame_gl::axis_color_x);

                    if (abs(pos.z) > 0.00001f)
                        debug_draw->text(vec3(pos.x, pos.y, 0.0f), std::to_string(pos.z), frame_gl::axis_color_z);
                    if (abs(pos.y) > 0.00001f)
                        debug_draw->text(vec3(pos.x, 0.0f, pos.z), std::to_string(pos.y), frame_gl::axis_color_y);
                    if (abs(pos.x) > 0.00001f)
                        debug_draw->text(vec3(0.0f, pos.y, pos.z), std::to_string(pos.x), frame_gl::axis_color_x);
                }
            }
        }

        void step_post(float dt) {
            for (auto manipulator : node<Transform, Manipulate3D>()) {
                auto transform = manipulator.get<Transform>();
                auto manipulate = manipulator.get<Manipulate3D>();

                // Update the transform
                transform->set_translation(manipulate->position());
                transform->set_rotation(manipulate->rotation());
            }
        }
    };
}