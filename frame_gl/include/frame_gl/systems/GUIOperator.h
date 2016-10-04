#pragma once
#include "frame/System.h"
#include "frame_gl/systems/Render.h"
#include "frame_gl/components/GUIRect.h"
#include "frame_gl/components/Camera.h"
#include "frame_gl/data/Shader.h"

namespace frame_gl
{
    FRAME_SYSTEM(GUIOperator, Node<GUIRect>) {
    public:
        GUIOperator(int gui_layer=1) : render(nullptr), gui_layer(gui_layer) {}
        ~GUIOperator() {}

    protected:
        void setup() {
            render = system<Render>();

            line_shader = new Shader(
                "Debug Line Shader",
                Shader::Preset::vert_standard(),
                Shader::Preset::frag_colors());
        }

        void teardown() {
            delete line_shader;
        }

        void step(float dt) {

            // Get the camera for the gui layer
            Camera* camera = render->display_camera(gui_layer);

            // Get the screen size
            vec2 screen_size = camera->target()->size();

            // Resize each root GUI rect
            for (auto rect : node<GUIRect>()) {
                if (rect->parent() == nullptr) {
                    rect->fit(vec2(0.0f), screen_size);
                }
            }

            // Get ready to render stuff
            camera->bind_target();
            line_shader->bind();
            line_shader->uniform(ShaderUniform::View, camera->view_matrix());
            line_shader->uniform(ShaderUniform::Projection, camera->projection_matrix());
            line_shader->uniform(ShaderUniform::Model, glm::mat4(1.0f));
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glDisable(GL_CULL_FACE);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            // Draw rectangles around each GUI element
            Mesh mesh;
            int index = 0;
            for (auto rect : node<GUIRect>()) {
                mesh.add_position(vec3(rect->top_left(), 0.0f));
                mesh.add_position(vec3(rect->top_right(), 0.0f));
                mesh.add_position(vec3(rect->bottom_right(), 0.0f));
                mesh.add_position(vec3(rect->bottom_left(), 0.0f));

                mesh.add_color(vec4(1.0f));
                mesh.add_color(vec4(1.0f));
                mesh.add_color(vec4(1.0f));
                mesh.add_color(vec4(1.0f));

                mesh.add_line(index, index+1);
                mesh.add_line(index+1, index+2);
                mesh.add_line(index+2, index+3);
                mesh.add_line(index+3, index);

                index += 4;
            }
            mesh.finalize();
            mesh.render();

            line_shader->unbind();
            camera->unbind_target();
        }

    private:
        Render* render;
        Shader* line_shader;
        int gui_layer;
    };
}