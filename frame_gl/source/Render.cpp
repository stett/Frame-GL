#define GLEW_STATIC
#include <string>
#include <GL/glew.h>
#include "frame/Log.h"
#include "frame/Frame.h"
#include "frame_gl/systems/Render.h"
using namespace frame;

void Render::step(float dt) {

    // For each camera, render all meshes
    _display_camera = nullptr;
    _display_target = nullptr;
    for (auto entity : node<Camera, RenderTarget>()) {

        // Render all meshes on this camera
        auto camera = entity.get<Camera>();
        camera->bind_target(auto_clear);
        for (auto object : node<MeshRenderer>())
            object->render(camera);
        camera->unbind_target();

        // If we haven't yet found a display target, look for one along with it's camera.
        if (!_display_target) {
            auto target = entity.get<RenderTarget>();
            if (target->display()) {
                _display_camera = camera;
                _display_target = target;
            }
        }
    }

    // Update the display target if we didn't find it yet. This will happen when
    // displaying camera-free render targets.
    if (!_display_target) {
        for (auto target : node<RenderTarget>()) {
            if (target->display()) {
                _display_target = target;
                break;
            }
        }
    }
}
