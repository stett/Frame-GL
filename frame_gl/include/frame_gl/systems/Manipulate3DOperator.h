#pragma once
#include "frame/System.h"
#include "frame_gl/components/Transform.h"
#include "frame_gl/components/Camera.h"
#include "frame_gl/systems/Input.h"
#include "frame_gl/math.h"
#include "frame_gl/components/Manipulate3D.h"
using namespace frame;

namespace frame
{
    FRAME_SYSTEM(Manipulate3DOperator, Node<Transform, Manipulate3D>) {

    public:
        void step(float dt) {

            auto input = frame()->systems().get<Input>();
            if (input == nullptr) return;

            for (auto manipulator : node<Transform, Manipulate3D>()) {
                auto transform = manipulator.get<Transform>();
                auto manipulate = manipulator.get<Manipulate3D>();

                // Update the manipulator's internal values
                if (input->mouse_down(manipulate->button()))
                    manipulate->shift(input->mouse_delta(), -10.0f * input->mouse_scroll().y);
                manipulate->update(dt);

                // Update the transform
                transform->set_translation(manipulate->position());
                transform->set_rotation(manipulate->rotation());
            }
        }
    };
}