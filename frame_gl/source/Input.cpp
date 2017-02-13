#include <bitset>
#include <string>
#include <locale>
#include <codecvt>
#include <iomanip>
#include <GLFW/glfw3.h>
#include "frame/Frame.h"
#include "frame/Log.h"
#include "frame_gl/systems/Input.h"
#include "frame_gl/systems/Window.h"
#include "frame_gl/math.h"
using namespace frame;

void Input::setup() {
    auto window = Window::get_window(frame());
    if (window == nullptr) return;
    window->keyboard.listen(this, &Input::keyboard_callback);
    window->character.listen(this, &Input::character_callback);
    window->mouse_button.listen(this, &Input::mouse_button_callback);
    window->mouse_position.listen(this, &Input::mouse_position_callback);
    window->mouse_scroll.listen(this, &Input::mouse_scroll_callback);
}

void Input::step_post() {
    mouse[Previous] = mouse[Current];
    mouse[Current] = mouse[Next];
    keyboard[Previous] = keyboard[Current];
    keyboard[Current] = keyboard[Next];
}

bool Input::mouse_down(MouseButton button, States state) const {
    return (mouse[state].buttons & (1 << (int)button)) != 0;
}

bool Input::mouse_up(MouseButton button, States state) const {
    return !mouse_down(button, state);
}

bool Input::mouse_clicked(MouseButton button) const {
    return mouse_down(button, Current) && mouse_up(button, Previous);
}

bool Input::mouse_released(MouseButton button) const {
    return mouse_up(button, Current) && mouse_down(button, Previous);
}

const vec2& Input::mouse_position(States state) const {
    return mouse[state].position;
}

vec2 Input::mouse_delta() const {
    return mouse[Current].position - mouse[Previous].position;
}

vec2 Input::mouse_scroll() const {
    return mouse[Current].scroll - mouse[Previous].scroll;
}

bool Input::key_down(int key, States state) const {
    return keyboard[state].keys[key];
}

bool Input::key_up(int key, States state) const {
    return !key_down(key, state);
}

bool Input::key_pressed(int key) const {
    return key_down(key, Current) && key_up(key, Previous);
}

bool Input::key_released(int key) const {
    return key_up(key, Current) && key_down(key, Previous);
}

void Input::keyboard_callback(int key, int scancode, int action, int mods) {

    if (capture_buffer && key != KEY_ENTER && key != KEY_GRAVE_ACCENT) return;

    if (key >= KeyboardState::NUM_KEYS) return;
    if (action == GLFW_PRESS) {
        keyboard[States::Next].keys[key] = true;
    } else if (action == GLFW_RELEASE) {
        keyboard[States::Next].keys[key] = false;
    }
}

void Input::character_callback(wchar_t codepoint) {
    if (!capture_buffer) return;
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    *capture_buffer << converter.to_bytes(codepoint);
}

void Input::mouse_button_callback(int button, int action, int mods) {
    if (action == GLFW_PRESS) {
        mouse[States::Next].buttons |= (1 << button);
    } else if (action == GLFW_RELEASE) {
        mouse[States::Next].buttons &= ~(1 << button);
    }
}

void Input::mouse_position_callback(const vec2& position) {
    mouse[States::Next].position = position;
}

void Input::mouse_scroll_callback(const vec2& delta) {
    mouse[States::Next].scroll += delta;
}
