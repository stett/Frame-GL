#pragma once
#include "frame/System.h"
#include "frame_gl/systems/Render.h"
#include "frame_gl/systems/Window.h"
#include "frame_gl/components/GUIRect.h"
#include "frame_gl/components/GUIClickable.h"
#include "frame_gl/components/Camera.h"
#include "frame_gl/data/Shader.h"

namespace frame_gl
{
    FRAME_SYSTEM(GUIOperator, Node<GUIRect>, Node<GUIRect, GUIClickable>) {
    public:
        GUIOperator(int gui_layer=1) : render(nullptr), gui_layer(gui_layer) {}
        ~GUIOperator() {}

    protected:
        void setup() {

            // Grab some related system references
            render = system<Render>();
            window = system<Window>();

            // Make a shader for rendering outlines
            line_shader = new Shader(
                "GUI Line Shader",
                Shader::Preset::vert_standard(),
                Shader::Preset::frag_colors());

            // Trigger callback whenever mouse moves
            window->mouse_position.listen(this, &GUIOperator::mouse_position_callback);
        }

        void teardown() {
            delete line_shader;
            window->mouse_position.ignore(this);
        }

        void step(float dt) {
            update_gui(dt);
            render_gui();
        }

    public:

        void mouse_position_callback(const vec2& screen_position) {
            mouse_position = screen_position;
        }

    private:

        void update_gui(float dt) {

            // Get the screen size
            Camera* camera = render->display_camera(gui_layer);
            vec2 screen_size = camera->target()->size();

            // Resize each root GUI rect
            for (auto rect : node<GUIRect>()) {
                if (rect->parent() == nullptr) {
                    rect->fit(vec2(0.0f), screen_size);
                }
            }

            // Mark each clickable rectangle as mouse in or mouse out,
            // and also find the focused clickable.
            focus = nullptr;
            for (auto e : node<GUIRect, GUIClickable>()) {

                // Get the components from this node element
                GUIRect* rect = e.get<GUIRect>();
                GUIClickable* clickable = e.get<GUIClickable>();

                // Set the clickable state, trigger callbacks, etc.
                clickable->set_mouse_in(rect->inside(mouse_position));

                // If this one has been marked as mouse-in, then it's focused
                if (clickable->mouse_in()) focus = e.entity();
            }
        }

        void render_gui() {

            // Get the camera for the gui layer
            Camera* camera = render->display_camera(gui_layer);

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
            size_t rect_count = node<GUIRect>().size();
            Mesh mesh(4 * rect_count, 4 * rect_count);
            int index = 0;
            for (auto rect : node<GUIRect>()) {
                vec4 color(vec3(rect.entity() == focus ? 1.0f : 0.4f), 1.0f);
                mesh.set_vertex(index + 0, vec3(rect->top_left(), 0.0f), color);
                mesh.set_vertex(index + 1, vec3(rect->top_right(), 0.0f), color);
                mesh.set_vertex(index + 2, vec3(rect->bottom_right(), 0.0f), color);
                mesh.set_vertex(index + 3, vec3(rect->bottom_left(), 0.0f), color);

                mesh.set_triangles({
                    ivec3(index, index+1, index+1),
                    ivec3(index+1, index+2, index+2),
                    ivec3(index+2, index+3, index+3),
                    ivec3(index+3, index+4, index+4),
                });

                index += 4;
            }
            mesh.render();

            line_shader->unbind();
            camera->unbind_target();
        }

    private:
        Render* render;
        Window* window;
        Shader* line_shader;
        Entity* focus;
        vec2 mouse_position;
        int gui_layer;
    };
}