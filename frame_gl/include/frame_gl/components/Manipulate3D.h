#pragma once
#include "frame/Component.h"
#include "frame_gl/systems/Input.h"
#include "frame_gl/math.h"
#include "glm/gtc/quaternion.hpp"
using namespace frame;

namespace frame
{
    FRAME_COMPONENT(Manipulate3D) {
    public:
        Manipulate3D(Input::MouseButton button=Input::MouseButton::Left, float distance=0.0f, const vec3& focus=vec3(0.0f), bool invert=false, float responsiveness=20.0f) :
            _button(button),
            _key(KEY_LEFT_ALT),
            invert(invert),
            distance(distance),
            pitch(0.0f),
            yaw(0.0f),
            target_distance(distance),
            target_pitch(pitch),
            target_yaw(yaw),
            pan_sensitivity(0.01f),
            scroll_sensitivity(0.0001f),
            _responsiveness(responsiveness),
            _position(focus + vec3(0.0f, 0.0f, distance)),
            _rotation(quat()),
            _focus(focus) {}

    public:
        Manipulate3D* set_pitch(float _pitch) {
            pitch = target_pitch = _pitch;
            return this;
        }

        Manipulate3D* set_yaw(float _yaw) {
            yaw = target_yaw = _yaw;
            return this;
        }

        Manipulate3D* shift_angle(const vec2& delta, float delta_zoom=0.0f) {
            float sign = invert ? -1.0f : 1.0f;
            target_distance += delta_zoom * sign * scroll_sensitivity * (max(100.0f, min(100000.0f, target_distance * target_distance)));
            target_yaw += delta.x * pan_sensitivity * sign;
            target_pitch += delta.y * pan_sensitivity * sign;
            if (target_distance < 0.0f) target_distance = 0.0f;
            if (target_pitch > pi * 0.5f) target_pitch = pi * 0.5f;
            if (target_pitch < -pi * 0.5f) target_pitch = -pi * 0.5f;
            return this;
        }

        Manipulate3D* shift_focus(vec3 delta) {
            delta *= (distance + 1.0f) * 0.001f;
            _target_focus += vec3(
               -delta.x * cos(yaw) - delta.z * sin(yaw),
                delta.y,
               -delta.x * sin(yaw) + delta.z * cos(yaw));
            return this;
        }

        Manipulate3D* set_focus(const vec3& position) {
            _target_focus = position;
            return this;

        }

        Manipulate3D* update(float dt) {
            float percent = min(1.0f, _responsiveness * dt);
            distance += (target_distance - distance) * percent;
            pitch += (target_pitch - pitch) * percent;
            yaw += (target_yaw - yaw) * percent;
            if (pitch < -pi * 0.5f) pitch = -pi * 0.5f;
            if (pitch > pi * 0.5f) pitch = pi * 0.5f;
            _focus += (_target_focus - _focus) * percent;
            _position = _focus + distance * vec3(-sin(yaw)*cos(pitch), sin(pitch), cos(yaw)*cos(pitch));
            _rotation = glm::angleAxis(-yaw, vec3(0.0f, 1.0f, 0.0f)) * glm::angleAxis(-pitch, vec3(1.0f, 0.0f, 0.0f));
            return this;
        }

        Input::MouseButton button() const { return _button; }

        int key() const { return _key; }

        const vec3& position() const { return _position; }

        const quat& rotation() const { return _rotation; }

        const vec3& focus() const { return _focus; }

        const vec3& target_focus() const { return _target_focus; }

        float responsiveness() const { return _responsiveness; }

    private:
        Input::MouseButton _button;
        int _key;
        bool invert;
        float distance;
        float pitch;
        float yaw;
        float target_distance;
        float target_pitch;
        float target_yaw;
        vec3 _target_focus;
        float pan_sensitivity;
        float scroll_sensitivity;
        float _responsiveness;
        vec3 _position;
        quat _rotation;
        vec3 _focus;
    };
}
