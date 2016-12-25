#pragma once
#include <limits>
#include "frame/System.h"
#include "frame/Log.h"
#include "frame_gl/systems/DebugDraw.h"
#include "frame_gl/math.h"
using namespace frame_gl;

namespace frame
{
    FRAME_SYSTEM(DisplayFPS) {
    public:
        DisplayFPS(float interval=2.0f) : total(0.0f), steps(0.0f), interval(interval), min(std::numeric_limits<float>::max()), max(0.0f) {}

    public:

        void setup() {
            debug_draw = frame()->systems().get<frame_gl::DebugDraw>();
        }

        void step(float dt) {
            total += dt;
            steps += 1.0f;

            if (dt < min) min = dt;
            if (dt > max) max = dt;

            if (debug_draw)
                debug_draw->screen_text(DebugDraw::Alignment::TopRight, glm::vec2(5.0f, 0.0f), status());

            if (total > interval) {
                average = total / steps;
                total = steps = 0.0f;
                min_average = min;
                max_average = max;
                min = std::numeric_limits<float>::max();
                max = 0.0f;

                if (!debug_draw)
                    Log::write(status());
            }
        }

    public:
        float fps() { return 1.0f / average; }
        float fps_min() { return 1.0f / max_average; }
        float fps_max() { return 1.0f / min_average; }
        float variance() { return (1.0f / min_average) - (1.0f / max_average); }
        std::string status() { return "FPS: " + std::to_string(int(fps())) + " (" + std::to_string(int(fps_min())) + "-" + std::to_string(int(fps_max())) + ")"; }

    private:
        float total;
        float steps;
        float interval;
        float average;
        float min, max;
        float min_average, max_average;
        frame_gl::DebugDraw* debug_draw;
    };
}