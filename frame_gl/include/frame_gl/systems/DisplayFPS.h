#pragma once
#include "frame/System.h"
#include "frame/Log.h"
#include "frame_gl/systems/DebugDraw.h"
#include "frame_gl/math.h"
using namespace frame_gl;

namespace frame
{
    FRAME_SYSTEM(DisplayFPS) {
    public:
        DisplayFPS(float interval=1.0f) : total(0.0f), steps(0.0f), interval(1.0f) {}

    public:

        void setup() {
            debug_draw = frame()->systems().get<frame_gl::DebugDraw>();
        }

        void step(float dt) {
            total += dt;
            steps += 1.0f;

            if (debug_draw)
                debug_draw->screen_text(DebugDraw::Alignment::TopLeft, glm::vec2(0.0f, 0.0f), std::to_string(fps()));

            if (total > interval) {
                average = total / steps;
                total = steps = 0.0f;

                if (!debug_draw)
                    Log::write("FPS: " + std::to_string(fps()));
            }
        }

    public:
        float fps() { return 1.0f / average; }

    private:
        float total;
        float steps;
        float interval;
        float average;
		frame_gl::DebugDraw* debug_draw;
    };
}