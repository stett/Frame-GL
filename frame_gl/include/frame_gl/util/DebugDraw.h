#pragma once
#include <memory>
#include "frame/System.h"
#include "frame_gl/math.h"

namespace frame_gl
{
    class DebugDraw : System {
    public:
        DebugDraw() {}
        ~DebugDraw() {}

    protected:
        void setup() {
            
        }

        void teardown() {
            delete mesh;
            delete shader;
        }

    public:
        void line();

    private:
        Mesh* mesh;
        Shader* shader;
    };
}