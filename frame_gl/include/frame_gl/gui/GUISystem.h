#pragma once
#include "frame/System.h"
#include "frame_gl/data/Shader.h"
#include "frame_gl/data/Mesh.h"
#include "frame_gl/gui/GUIRect.h"
#include "frame_gl/gui/GUITransform.h"
#include "frame_gl/systems/DebugDraw.h"

namespace frame
{
    FRAME_SYSTEM(GUISystem, Node<GUIRect>) {
    public:
        GUISystem(int gui_layer=1) : render(nullptr), gui_layer(gui_layer) {}
        ~GUISystem() {}

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

            rect_mesh = new Mesh(POSITION_VEC3, 4, 4);
            rect_mesh->set_vertices({ vec3(0.0f), vec3(1.0f, 0.0f, 0.0f), vec3(1.0f, 1.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f) });
            rect_mesh->set_triangles({ ivec3(0, 1, 1), ivec3(1, 2, 2), ivec3(2, 3, 3), ivec3(3, 0, 0) });

            // Trigger callback whenever mouse moves
            window->mouse_position.listen(this, &GUISystem::mouse_position_callback);
        }

        void teardown() {
            delete line_shader;
            delete rect_mesh;
            window->mouse_position.ignore(this);
        }

        void step() {
            update_gui();
            render_gui();
        }

    public:

        void mouse_position_callback(const vec2& screen_position) {
            mouse_position = screen_position;
        }

    private:

        void update_gui() {

            // Get the screen size
            Camera* camera = render->display_camera(gui_layer);
            vec2 screen_size = camera->target()->size();

            // Resize each GUI rect
            for (auto rect : node<GUIRect>()) {
                rect->update(dt());
            }

            /*
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
            */
        }

        void render_gui() {

            // Get the camera for the gui layer
            Camera* camera = render->display_camera(gui_layer);

            // Get ready to render stuff
            camera->bind_target();
            line_shader->bind();
            line_shader->uniform(ShaderUniform::View, camera->view_matrix());
            line_shader->uniform(ShaderUniform::Projection, camera->projection_matrix());

            // Set up GL
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glDisable(GL_CULL_FACE);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glLineWidth(2.0f);

            // Draw rectangles around each GUI element
            mat4 transform;
            rect_mesh->bind();
            for (auto rect : node<GUIRect>()) {
                line_shader->uniform(ShaderUniform::Model, rect->matrix());
                rect_mesh->render();
            }
            rect_mesh->unbind();

            line_shader->unbind();
            camera->unbind_target();
        }

    private:
        Render* render;
        Window* window;
        Shader* line_shader;
        Mesh* rect_mesh;
        Entity* focus;
        vec2 mouse_position;
        int gui_layer;
    };
}