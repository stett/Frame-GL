#pragma once
#include "frame/System.h"
#include "frame/Entity.h"
#include "frame_gl/math.h"

namespace frame
{
    FRAME_SYSTEM(Planes) {
    protected:
        void setup();
        void teardown();

    private:
        Entity* plane;
    };
}