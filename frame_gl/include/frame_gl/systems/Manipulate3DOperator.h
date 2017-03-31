#pragma once
#include <string>
#include "frame/System.h"
#include "frame_gl/components/Transform.h"
#include "frame_gl/components/Camera.h"
#include "frame_gl/systems/Input.h"
#include "frame_gl/systems/DebugDraw.h"
#include "frame_gl/systems/UIText.h"
#include "frame_gl/components/Manipulate3D.h"
#include "frame_gl/math.h"
#include "frame_gl/colors.h"
using namespace frame;

namespace frame
{
    FRAME_SYSTEM(Manipulate3DOperator, Node<Transform, Manipulate3D>) {

    public:
        Manipulate3DOperator() : _ui_text_color(1.0f, 0.5f, 0.25f, 1.0f) {}

    public:
        void step() {

            auto input = frame()->systems().get<Input>();
            if (input == nullptr) return;

            auto debug_draw = system<frame_gl::DebugDraw>();
            auto ui_text = system<frame_gl::UIText>();
            glm::vec4 ui_text_color(1.0f, 0.5f, 0.25f, 1.0f);

            for (auto manipulator : node<Transform, Manipulate3D>()) {
                auto transform = manipulator.get<Transform>();
                auto manipulate = manipulator.get<Manipulate3D>();

                // Only control this object if the manipulation button is being held
                //if (input->mouse_down(manipulate->button())) {
                if (input->key_down(KEY_TAB)) {

                    // Add control prompts
                    if (ui_text) {
                        ui_text->add_line("Zoom              Mouse Wheel", ui_text_color);
                        ui_text->add_line("Move              W/A/S/D", ui_text_color);
                        ui_text->add_line("Move Fast         Left Shift", ui_text_color);
                        ui_text->add_line("Raise/Lower       E/Q", ui_text_color);
                        ui_text->add_line("Re-center         Space", ui_text_color);
                    }

                    // Adjust zoom & scroll speed with shift
                    float speed = input->key_down(KEY_LEFT_SHIFT) ? 5.0f : 1.0f;

                    // Angle manipulation
                    manipulate->shift_angle(input->mouse_delta(), -10.0f * speed * input->mouse_scroll().y);

                    // Translation manipulation
                    glm::vec3 focus_delta;
                    if (input->key_down(KEY_W)) focus_delta.z -= speed;
                    if (input->key_down(KEY_S)) focus_delta.z += speed;
                    if (input->key_down(KEY_A)) focus_delta.x += speed;
                    if (input->key_down(KEY_D)) focus_delta.x -= speed;
                    if (input->key_down(KEY_E)) focus_delta.y += speed;
                    if (input->key_down(KEY_Q)) focus_delta.y -= speed;
                        manipulate->shift_focus(focus_delta);

                    if (input->key_pressed(KEY_SPACE))
                        manipulate->set_focus(vec3(0.0f));
                } else {

                    // Add mouse control prompt
                    if (ui_text)
                        ui_text->add_line(std::string("Manipulate view   ") + (manipulate->button() == Input::MouseButton::Left ? "Left Mouse Button" : "Right Mouse Button"), ui_text_color);
                }

                // Interpolate motion
                manipulate->update(dt());

                // If we have a debug module, draw the focus point
                if (debug_draw) {
                    vec3 pos = manipulate->focus();
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
            }
        }

        void step_post() {
            for (auto manipulator : node<Transform, Manipulate3D>()) {
                auto transform = manipulator.get<Transform>();
                auto manipulate = manipulator.get<Manipulate3D>();

                // Update the transform
                transform->set_translation(manipulate->position());
                transform->set_rotation(manipulate->rotation());
            }
        }

    public:

        const vec4& ui_text_color() { return _ui_text_color; }

    private:

        vec4 _ui_text_color;
    };
}