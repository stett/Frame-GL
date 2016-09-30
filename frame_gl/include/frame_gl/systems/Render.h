#pragma once
#include <vector>
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
        const unsigned int MAX_LAYERS = 32;

    public:
        Render(bool auto_clear=true)
        : display_targets(std::vector<RenderTarget*>(MAX_LAYERS, nullptr)), display_cameras(std::vector<Camera*>(MAX_LAYERS, nullptr)),/*_display_target(nullptr), _display_camera(nullptr), */auto_clear(auto_clear) {}

    public:

        /// \brief Returns a pointer to the first RenderTarget component found in the frame which has its display flag on.
        //RenderTarget* display_target() { return _display_target; }

        /// \brief Returns a pointer to the Camera, if there is one, which renders to the RenderTarget returned by display_target().
        Camera* display_camera(unsigned int layer=0) {
        //Camera* display_camera() {
            return (layer < MAX_LAYERS) ? display_cameras[layer] : nullptr;
            //return _display_camera;
        }

        RenderTarget* display_target(unsigned int layer=0) {
            return (layer < MAX_LAYERS) ? display_targets[layer] : nullptr;
        }

    protected:
        virtual void step(float dt);

    private:
        /*
        RenderTarget* _display_target;
        Camera* _display_camera;
        */
        std::vector<RenderTarget*> display_targets;
        std::vector<Camera*> display_cameras;
        bool auto_clear;
    };
}