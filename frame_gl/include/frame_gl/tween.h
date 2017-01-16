#pragma once
#include <limits>
#include "frame_gl/math.h"

#define TWEEN_FUNC(NAME)                                                                \
    template <typename ValueT, typename TimeT>                                          \
    inline void NAME(ValueT& result, const ValueT& a, const ValueT& b, TimeT t)         \

namespace frame
{
    namespace tween
    {
        TWEEN_FUNC(constant) {
            result = a;
        }

        TWEEN_FUNC(linear) {
            result = a + ((b - a) * t);
        }

        TWEEN_FUNC(spherical) {
            auto a_dot_b = dot(a, b);
            if (a_dot_b > 1.0f) a_dot_b -= 2.0f;
            if (a_dot_b < -1.0f) a_dot_b += 2.0f;
            auto angle = acos(a_dot_b);
            if (angle < std::numeric_limits<float>::min()) { result = a; return; }
            if (frame::pi - angle < std::numeric_limits<float>::min()) { result = b; return; }
            float a_coeff = sin(angle * (1.0f - t));
            float b_coeff = sin(angle * t);
            float sin_inv = 1.0f / sin(angle);
            result = normalize(((a * a_coeff) + (b * b_coeff)) * sin_inv);
        }

        TWEEN_FUNC(cubic) {
            linear(result, a, b, (-2*t+3)*t*t);
        }

        TWEEN_FUNC(sinusoidal) {
            linear(result, a, b, sin(t * pi * 0.5f));
        }
    }
}

#undef TWEEN_FUNC