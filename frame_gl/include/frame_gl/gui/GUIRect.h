#pragma once
#include "frame/Component.h"
#include "frame_gl/gui/GUITransform.h"
#include "frame_gl/math.h"

namespace frame
{
    FRAME_COMPONENT(GUIRect)
    {
    public:

        GUIRect() : _time(1.0f), _duration(1.0f), _slot(nullptr) {}
        GUIRect(const GUISlot* slot) : _slot(slot), _time(1.0f), _duration(1.0f) {}

    public:

        GUIRect* set_slot(const GUISlot* slot) {
            _slot = slot;
            return this;
        }

        GUIRect* set_tween(std::function<void (float&, const float&, const float&)> tween) {
            _tween = tween;
        }

    public:

        const mat4& matrix() const { return _matrix; }
        GUITransform& transform() { return _transform; }
        const GUITransform& transform() const { return _transform; }
        const GUISlot* slot() const { return _slot; }
        std::function<void (float&, const float&, const float&)> tween() { return _tween; }

    public:

        void set_target(const GUITransform& target, float duration) {
            _duration = duration;
        }

        void set_target(const GUITransform& target) {
            endpoints[0] = _transform;
            endpoints[1] = target;
            _time = 0.0f;
        }

        void set_transform(const GUITransform& transform) {
            endpoints[0] = endpoints[1] = transform;
            _transform = transform;
            _time = _duration;
        }

        bool done_animating() {
            return _duration - _time < std::numeric_limits<float>::epsilon();
        }

        void update(float dt) {

            // Update the transform if there's any tweening left to do.
            if (!done_animating()) {
                float t = min(1.0f, _time / _duration);
                _transform.lerp(endpoints[0], endpoints[1], t);
                _time += dt;
            }

            // Update the transform matrix
            _matrix = _slot ? _slot->transform * _transform.matrix() : _transform.matrix();
        }

    private:

        const GUISlot* _slot;
        vec2 _area[2];
        GUITransform _transform;
        GUITransform endpoints[2];
        float _time;
        float _duration;
        mat4 _matrix;
        std::function<void (float&, const float&, const float&)> _tween;
    };
}