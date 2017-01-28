#pragma once
#include "frame/Component.h"
#include "frame_gl/gui/GUIRect.h"

namespace frame
{
    FRAME_COMPONENT(GUIButton, GUIRect)
    {
    public:
        GUIButton() {}
        GUIButton(std::function<void ()> on_click) : on_click(on_click) {}

    public:
        void set_mouse_in(bool in) {

            hover.scale = 0.9f;

            if (!mouse_in) {
                get<GUIRect>()->set_target(hover);
            }

            mouse_in = in;
        }

    private:
        bool mouse_in;
        std::function<void ()> on_click;
        GUITransform hover;
    }
}