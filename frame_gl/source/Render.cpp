#define GLEW_STATIC
#include <string>
#include <GL/glew.h>
#include "frame/Log.h"
#include "frame/Frame.h"
#include "frame_gl/systems/Render.h"
using namespace frame;

void Render::step() {

    // For each camera, render all meshes
    for (auto entity : node<Camera, RenderTarget>()) {

        // Render all meshes on this camera
        auto camera = entity.get<Camera>();
        auto target = entity.get<RenderTarget>();
        target->bind_target(auto_clear);


        // Draw all the meshes in their own modes
        for (auto object : node<MeshRenderer>()) {
            if (camera->has_layer(object->layer())) {
                object->bind(camera);

                // Should find a better way to do this...
                if (_mode == Wireframe)
                    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

                object->render(camera);
                object->unbind();
            }
        } 

        // Unbind the render target
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

void Render::load_prototypes(std::back_insert_iterator< std::vector< CommandPrototype > >& commands) {
    *(commands++) = {
        Command("render", "[stats|wires]"),
        "List statistics and toggle wireframe",
        ""
    };
}

void Render::handle(Command command) {
    if (command.arg_count() == 0) {
        command.add_result_line("Please specify an option");
        return;
    }

    if (command.arg(0) == "stats") {
        command.add_result_line("Render targets: " + std::to_string(node<RenderTarget>().size()));
        command.add_result_line("Mesh Renderers: " + std::to_string(node<MeshRenderer>().size()));

    } else if (command.arg(0) == "wires") {
        if (_mode == Normal) {
            _mode = Wireframe;
            command.add_result_line("Wireframes On");
        } else {
            _mode = Normal;
            command.add_result_line("Wireframes Off");
        }
    }
}
