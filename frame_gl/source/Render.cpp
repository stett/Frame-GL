#define GLEW_STATIC
#include <string>
#include <GL/glew.h>
#include "frame/Log.h"
#include "frame/Frame.h"
#include "frame_gl/systems/Render.h"
using namespace frame;

void Render::step() {

    // For each camera, render all meshes
    //_display_camera = nullptr;
    //_display_target = nullptr;
    for (auto entity : node<Camera, RenderTarget>()) {

        // Render all meshes on this camera
        auto camera = entity.get<Camera>();
        auto target = entity.get<RenderTarget>();
        target->bind_target(auto_clear);
        for (auto object : node<MeshRenderer>())
            if (camera->has_layer(object->layer()))
                object->render(camera);
        target->unbind_target();

        // Update the display camera for this layer
        int layer = target->display_layer();
        if (layer != -1) {
            //display_targets[layer] = target;
            display_cameras[layer] = camera;
        }

        // If we haven't yet found a display target, look for one along with it's camera.
        /*
        if (!_display_target) {
            auto target = entity.get<RenderTarget>();
            if (target->display()) {
                _display_camera = camera;
                _display_target = target;
            }
        }
        */
    }

    // Update display targets
    for (auto target : node<RenderTarget>()) {
        int layer = target->display_layer();
        if (layer != -1)
            display_targets[layer] = target;
    }

    // Update the display target if we didn't find it yet. This will happen when
    // displaying camera-free render targets.
    /*
    if (!_display_target) {
        for (auto target : node<RenderTarget>()) {
            if (target->display()) {
                _display_target = target;
                break;
            }
        }
    }
    */
}
