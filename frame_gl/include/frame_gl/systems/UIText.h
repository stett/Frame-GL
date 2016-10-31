#pragma once
#include <queue>
#include <string>
#include <utility>
#include "frame/System.h"
#include "frame_gl/math.h"

namespace frame_gl
{
    FRAME_SYSTEM(UIText) {
    public:
        UIText() : size(16.0f), spacing(1.0f) {}

    public:
        void setup() {
            debug_draw = frame()->systems().get<frame_gl::DebugDraw>();
        }

        void step(float dt) {
            position = glm::vec2(10.0f, 0.0f);
            while (!lines.empty()) {
                debug_draw->screen_text(DebugDraw::Alignment::TopLeft, position, lines.front().second, lines.front().first, size);
                lines.pop();
                position.y += size * spacing;
            }
        }

        void add_line(const std::string& line, const glm::vec4& color=glm::vec4(1.0f)) {
            lines.push(std::pair<glm::vec4, std::string>(color, line));
        }

    private:
        frame_gl::DebugDraw* debug_draw;
        std::queue< std::pair< glm::vec4, std::string > > lines;
        glm::vec2 position;
        float size;
        float spacing;
    };
}