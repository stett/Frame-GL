#pragma once
#include "frame/Component.h"
#include "frame/Log.h"
#include "frame_gl/components/GUIRect.h"

namespace frame_gl
{
    FRAME_COMPONENT(GUIClickable, GUIRect) {
    public:
        GUIClickable() : _mouse_in(false) {}
        ~GUIClickable() {}

    public:
        bool mouse_in() { return _mouse_in; }
        void set_mouse_in(bool mouse_in) {
            if (mouse_in && !_mouse_in) {

                // TODO: On-enter stuff

            } else if (_mouse_in && !mouse_in) {

                // TODO: On-exit stuff
            }

            _mouse_in = mouse_in;
        }

    private:
        bool _mouse_in;
    };
}