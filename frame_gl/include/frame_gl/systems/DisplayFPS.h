#pragma once
#include "frame/System.h"
#include "frame/Log.h"

namespace frame
{
    FRAME_SYSTEM(DisplayFPS) {
    public:
        DisplayFPS(float interval=1.0f) : total(0.0f), steps(0.0f), interval(1.0f) {}

    public:
        void step(float dt) {
            total += dt;
            steps += 1.0f;
            if (total > interval) {
                average = total / steps;
                total = steps = 0.0f;
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
    };
}