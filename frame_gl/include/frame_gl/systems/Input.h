#pragma once
#include <bitset>
#include <GLFW/glfw3.h>
#include "frame/System.h"
#include "frame_gl/systems/Window.h"
#include "frame_gl/math.h"

namespace frame
{
    FRAME_SYSTEM(Input) {
    public:
        struct MouseState {
            vec2 position;
            vec2 scroll;
            int buttons;
            MouseState() : position(vec2(0.0f)), scroll(vec2(0.0f)), buttons(0) {}
        };

        enum MouseButton {
            Left = GLFW_MOUSE_BUTTON_1,
            Right = GLFW_MOUSE_BUTTON_2,
            Middle = GLFW_MOUSE_BUTTON_3,
            Button4 = GLFW_MOUSE_BUTTON_4,
            Button5 = GLFW_MOUSE_BUTTON_5,
            Button6 = GLFW_MOUSE_BUTTON_6,
            Button7 = GLFW_MOUSE_BUTTON_7,
            Button8 = GLFW_MOUSE_BUTTON_8
        };

        struct KeyboardState {
            static const std::size_t NUM_KEYS = 512;
            std::bitset<NUM_KEYS> keys;
            KeyboardState() : keys(0) {}
        };

        enum States { Previous = 0, Current, Count };

    protected:
        void setup();
        void step_post(float dt);

    public:
        bool mouse_down(MouseButton button, States state=Current) const;
        bool mouse_up(MouseButton button, States state=Current) const;
        bool mouse_clicked(MouseButton button) const;
        bool mouse_released(MouseButton button) const;
        const vec2& mouse_position(States state=Current) const;
        vec2 mouse_delta() const;
        vec2 mouse_scroll() const;

        bool key_down(int key, States state=Current) const;
        bool key_up(int key, States state=Current) const;
        bool key_pressed(int key) const;
        bool key_released(int key) const;

    public:
        void keyboard_callback(int key, int scancode, int action, int mods);
        void mouse_button_callback(int button, int action, int mods);
        void mouse_position_callback(const vec2& position);
        void mouse_scroll_callback(const vec2& delta);

    private:
        MouseState mouse[States::Count];
        KeyboardState keyboard[States::Count];
    };
}