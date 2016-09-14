#pragma once
#include "frame/System.h"
#include "frame/Node.h"
#include "frame_gl/components/Transform.h"
#include "frame_gl/math.h"
#include "frame_gl_demo/components/Rotisserie.h"
using namespace frame;

namespace frame_demo
{
    FRAME_SYSTEM(RotisserieOperator, Node<Rotisserie, Transform>) {
    public:
        RotisserieOperator() : angle(0.0f), axis(vec3(0.0f, 1.0f, 0.0f)) {}

    public:
        void step(float dt) {
            angle += 1.0f * dt;

            for (auto rotisserie : node<Rotisserie, Transform>()) {
                auto t = rotisserie.get<Transform>();
                t->set_rotation(axis, angle);
            }
        }

    private:
        float angle;
        vec3 axis;
    };
}