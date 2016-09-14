#pragma once
#include "frame/System.h"
#include "frame_gl/components/Camera.h"
#include "frame_gl/components/Transform.h"
#include "frame_gl/components/MeshRenderer.h"
#include "frame_gl/math.h"

namespace frame
{
    /// \class Render
    /// \brief Draws all MeshRenderer components to all Camera target textures.
    FRAME_SYSTEM(Render, Node<RenderTarget>, Node<Camera, RenderTarget>, Node<MeshRenderer>) {
    public:
        Render(bool auto_clear=true) : _display_target(nullptr), _display_camera(nullptr), auto_clear(auto_clear) {}

    public:

        /// \brief Returns a pointer to the first RenderTarget component found in the frame which has its display flag on.
        RenderTarget* display_target() { return _display_target; }

        /// \brief Returns a pointer to the Camera, if there is one, which renders to the RenderTarget returned by display_target().
        Camera* display_camera() { return _display_camera; }

    protected:
        virtual void step(float dt);

    private:
        RenderTarget* _display_target;
        Camera* _display_camera;
        bool auto_clear;
    };
}