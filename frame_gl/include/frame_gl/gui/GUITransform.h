#pragma once
#include "frame_gl/tween.h"
#include "frame_gl/math.h"

namespace frame
{

    struct GUISlot
    {
        vec2 bounds[2];
        vec4 padding;
        mat4 transform;
    };

    struct GUITransform
    {
        float scale;
        quat rotation;
        vec3 offset;
        vec3 translation;

        const mat4& matrix() const {
            _matrix = glm::scale(glm::translate(mat4(1.0f), translation + offset) * rotation.matrix(), vec3(scale));
            return _matrix;
        }

        void lerp(const GUITransform& a, const GUITransform& b, float t) {
            tween::linear(scale, a.scale, b.scale, t);
            tween::linear(offset, a.offset, b.offset, t);
            tween::linear(translation, a.translation, b.translation, t);
            tween::spherical(rotation, a.rotation, b.rotation, t);
        }

    private:
        mutable mat4 _matrix;
    };
}