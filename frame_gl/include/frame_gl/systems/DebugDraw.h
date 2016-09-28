#pragma once
#include <memory>
#include <queue>
#include <utility>
#include "frame/System.h"
#include "frame_gl/systems/Render.h"
#include "frame_gl/components/Camera.h"
#include "frame_gl/data/Mesh.h"
#include "frame_gl/data/Shader.h"
#include "frame_gl/math.h"
#include "glm/gtc/matrix_transform.hpp"

namespace frame_gl
{
    FRAME_SYSTEM(DebugDraw) {
    public:
        DebugDraw() {}
        ~DebugDraw() {}

    protected:
        void setup() {

            render = frame()->systems().get<Render>();

            shape_shader = new Shader(
                "Debug Shape Shader",
                Shader::Preset::vert_standard(),
                Shader::Preset::frag_normals());

            text_shader = new Shader(
                "Debug Text Shader",

                Resource<ShaderPart>(ShaderPart::Type::Vertex,
                    "#version 330\n                                                 "
                    "layout(location = 0)in vec3 vert_position;                     "
                    //"layout(location = 1)in vec3 vert_normal;                       "
                    //"layout(location = 2)in vec2 vert_uv;                           "
                    //"layout(location = 3)in vec4 vert_color;                        "
                    "uniform mat4 model;                                            "
                    "uniform mat4 view;                                             "
                    "uniform mat4 projection;                                       "
                    "out vec4 geom_position;                                        "
                    //"out vec3 frag_normal;                                          "
                    //"out vec2 frag_uv;                                              "
                    //"out vec4 frag_color;                                           "
                    "void main() {                                                  "
                    "    mat4 transform = projection * view * model;                "
                    "    geom_position  = transform * vec4(vert_position, 1.0);     "
                    //"    frag_normal    = vert_normal * inverse(mat3(model));       "
                    //"    frag_uv        = vert_uv;                                  "
                    //"    frag_color     = vert_color;                               "
                    "    gl_Position    = geom_position;                            "
                    "}                                                              "
                ),

                Resource<ShaderPart>(ShaderPart::Type::Geometry,
                    "#version 330\n"
                    "layout(lines) in;"
                    "layout(line_strip, max_vertices = 16) out;"
                    "in vec4 geom_position[2];"
                    "void main() {"
                    "   float size = 0.05f;"
                    "   float scale = geom_position[0].w * size;"
                    "   vec4 pos = geom_position[0];"
                    "   gl_Position = pos;"
                    "   EmitVertex();"
                    "   gl_Position = pos + vec4(.5, 1, 0, 0) * scale;"
                    "   EmitVertex();"
                    "   gl_Position = pos + vec4(1, 0, 0, 0) * scale;"
                    "   EmitVertex();"
                    "   gl_Position = pos + vec4(.75, .5, 0, 0) * scale;"
                    "   EmitVertex();"
                    "   gl_Position = pos + vec4(.25, .5, 0, 0) * scale;"
                    "   EmitVertex();"
                    "   EndPrimitive();"
                    "}"
                ),

                Resource<ShaderPart>(ShaderPart::Type::Fragment,
                    "#version 330\n                 "
                    "out vec4 pixel_color;          "
                    "void main() {                  "
                    "    pixel_color = vec4(1, 0, 0, 1);"
                    "}                              "
                )
            );
        }

        void teardown() {
            delete shape_shader;
            delete text_shader;
        }

        void step(float dt) {

            // Bind the render target
            Camera* camera = render->display_camera();
            camera->bind_target();

            // Draw shit
            render_lines(camera);
            render_cubes(camera);
            render_text(camera);

            // Tear down
            camera->unbind_target();
        }

    private:

        void render_lines(Camera* camera) {

            if (lines.empty())
                return;

            // Bind the shape shader
            shape_shader->bind();
            shape_shader->uniform(shape_shader->uniforms().view, camera->view_matrix());
            shape_shader->uniform(shape_shader->uniforms().projection, camera->projection_matrix());

            // Just need one model matrix
            shape_shader->uniform(shape_shader->uniforms().model, glm::mat4(1.0f));

            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glDisable(GL_CULL_FACE);
            glLineWidth(2.0f);

            // Build a mesh with a bunch of lines
            Mesh mesh;
            int mesh_indices = 0;
            while (!lines.empty()) {
                auto& line = lines.front();
                mesh.add_position(line.first);
                mesh.add_position(line.second);
                mesh.add_normal(glm::vec3(1.0f));
                mesh.add_normal(glm::vec3(1.0f));
                mesh.add_line(mesh_indices, mesh_indices+1);
                mesh_indices += 2;
                lines.pop();
            }

            // Draw it
            mesh.finalize();
            mesh.render();

            shape_shader->unbind();
        }

        void render_cubes(Camera* camera) {

            if (cubes.empty())
                return;

            // Bind the shape shader
            shape_shader->bind();
            shape_shader->uniform(shape_shader->uniforms().view, camera->view_matrix());
            shape_shader->uniform(shape_shader->uniforms().projection, camera->projection_matrix());

            Resource<Mesh> mesh = Mesh::Factory::cube(1.0f);

            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glEnable(GL_CULL_FACE);

            while (!cubes.empty()) {
                shape_shader->uniform(shape_shader->uniforms().model, cubes.front());
                mesh->render();
                cubes.pop();
            }

            shape_shader->unbind();
        }

        void render_text(Camera* camera) {

            if (strings.empty())
                return;

            // Bind the text shader
            text_shader->bind();
            text_shader->uniform(shape_shader->uniforms().view, camera->view_matrix());
            text_shader->uniform(shape_shader->uniforms().projection, camera->projection_matrix());

            /*
            int view_loc = text_shader->locate("view");
            int model_loc = text_shader->locate("model");
            int projection_loc = text_shader->locate("projection");
            */

            //int character_number_location = text_shader->locate("character_number");
            //text_shader->uniform(character_number_location, 0);

            Mesh mesh;

            mesh.add_position(vec3(0.0f));
            mesh.add_position(vec3(1.0f));
            mesh.add_line(0, 1);
            mesh.finalize();

            //int uniform_character_number = text_shader->locate("character_number");
            //text_shader->uniform(uniform_character_number, 0);

            glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
            glDisable(GL_CULL_FACE);
            glLineWidth(1.0f);

            while (!strings.empty()) {
                glm::mat4 model = glm::translate(glm::mat4(1.0f), strings.front().first);
                text_shader->uniform(text_shader->uniforms().model, model);

                int i = 0;
                for (char c : strings.front().second) {
                    //text_shader->uniform(uniform_character_number, i++);
                    //text_shader->uniform("character_color", vec3(1, 0, 0));
                    mesh.render();
                }

                strings.pop();
            }

            text_shader->unbind();
        }

    public:

        void line(const glm::vec3& p0, const glm::vec3& p1) {
            lines.push(std::make_pair(p0, p1));
        }

        void cube(const glm::vec3& point, float scale=1.0f) {
            cube(glm::scale(glm::translate(glm::mat4(1.0f), point), glm::vec3(scale)));
        }

        void cube(const glm::mat4& transform) {
            cubes.push(transform);
        }

        void string(const glm::vec3& position, const std::string& str) {
            strings.push(std::make_pair(position, str));
        }

    private:
        Render* render;
        Shader* shape_shader;
        Shader* text_shader;
        int text_shader_characters;
        std::queue< std::pair< glm::vec3, glm::vec3 > > lines;
        std::queue< glm::mat4 > cubes;
        std::queue< std::pair< glm::vec3, std::string > > strings;
    };
}