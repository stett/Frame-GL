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
                Shader::Preset::vert_standard(),
                Resource<ShaderPart>(ShaderPart::Geometry,
                    "#version 330\n                                     "
                    "layout(lines) in;                                  "
                    "layout(line_strip, max_vertices = 16) out;         "
                    "uniform mat4 model;                                            "
                    "uniform mat4 view;                                             "
                    "uniform mat4 projection;                                       "
                    //"uniform float character_number;                                  "
                    "in vec4 frag_position[];                           "
                    "void main() {                                      "
                    "   float size = 0.1f;"

                    "   mat4 transform = projection * view * model;                "
                    "   mat4 trans_inv = inverse(transform);           "
                    "   mat4 view_inv = inverse(view);"
                    "   mat4 proj_inv = inverse(projection);"
                    "   float scale = frag_position[0].w * size;"
                    //"   vec4 pos = frag_position[0] + vec4(character_number * scale, 0, 0, 0);"
                    "   vec4 pos = frag_position[0];"

                    "   /* A */                                         "
                    "   gl_Position = pos;                 "
                    "   EmitVertex();                                   "
                    "   gl_Position = pos + vec4(.5, 1, 0, 0) * scale;"
                    "   EmitVertex();                                   "
                    "   gl_Position = pos + vec4(1, 0, 0, 0) * scale;"
                    "   EmitVertex();                                   "
                    "   gl_Position = pos + vec4(.75, 0.5, 0, 0) * scale;"
                    "   EmitVertex();                                   "
                    "   gl_Position = pos + vec4(.25, 0.5, 0, 0) * scale;"
                    "   EmitVertex();                                   "
                    "   EndPrimitive();                                 "
                    "}                                                  "
                ),
                Shader::Preset::frag_white());

            /*
            text_shader = new Shader(
                "Debug Text Shader",

                Resource<ShaderPart>(ShaderPart::Type::Vertex,
                    "#version 330\n                                                 "
                    "layout(location = 0)in vec3 vert_position;                     "
                    "layout(location = 1)in vec3 vert_normal;                       "
                    "uniform mat4 model;                                            "
                    "uniform mat4 view;                                             "
                    "uniform mat4 projection;                                       "
                    "out vec3 geom_normal;                                          "
                    "out vec3 geom_position;                                        "
                    "void main() {                                                  "
                    "   geom_position = vert_position;                              "
                    "   geom_normal = vert_normal;                                  "
                    "}                                                              "
                ),

                Resource<ShaderPart>(ShaderPart::Type::Geometry,
                    "#version 330\n                                             "
                    "layout(points) in;                                         "
                    "layout(points, max_vertices = 256) out;                    "
                    "uniform mat4 model;                                        "
                    "uniform mat4 view;                                         "
                    "uniform mat4 projection;                                   "
                    //"uniform int characters[256];                               "
                    "in vec4 geom_position;                                     "
                    "                                                           "
                    "void main() {                                              "
                    "   for (int i = 0; i < 256; ++i) {                         "
                    "       gl_Position = geom_position + vec4(2*i, 0, 0, 0);   "
                    "       EmitVertex();                                       "
                    "       gl_Position = geom_position + vec4(2*i+1, 0, 0, 0); "
                    "       EmitVertex();                                       "
                    "   }                                                       "
                    "}                                                          "
                    "EndPrimitive();                                            "
                    "                                                           "
                    ),
                Shader::Preset::frag_white());
                */

            //text_shader_character = text_shader.locate("character");
        }

        void teardown() {
            delete shape_shader;
            delete text_shader;
        }

        void step(float dt) {

            Camera* camera = render->display_camera();
            camera->bind_target();

            // Bind the shape shader
            shape_shader->bind();
            shape_shader->uniform(shape_shader->uniforms().view, camera->view_matrix());
            shape_shader->uniform(shape_shader->uniforms().projection, camera->projection_matrix());

            // Draw lines
            render_lines();

            // Draw cubes
            render_cubes();

            // Bind the text shader
            text_shader->bind();
            text_shader->uniform(shape_shader->uniforms().view, camera->view_matrix());
            text_shader->uniform(shape_shader->uniforms().projection, camera->projection_matrix());

            // Draw text
            render_text();

            // Tear down
            Shader::unbind_all();
            camera->unbind_target();
        }

    private:

        void render_lines() {

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
        }

        void render_cubes() {
            Resource<Mesh> mesh = Mesh::Factory::cube(1.0f);

            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glEnable(GL_CULL_FACE);

            while (!cubes.empty()) {
                shape_shader->uniform(shape_shader->uniforms().model, cubes.front());
                mesh->render();
                cubes.pop();
            }
        }

        void render_text() {
            Mesh mesh;

            mesh.add_position(vec3(0.0f));
            mesh.add_position(vec3(0.0f));
            mesh.add_line(0, 0);

            mesh.finalize();

            glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
            glDisable(GL_CULL_FACE);
            glLineWidth(1.0f);
            while (!strings.empty()) {
                glm::mat4 model = glm::translate(glm::mat4(1.0f), strings.front().first);
                text_shader->uniform(text_shader->uniforms().model, model);

                int i = 0;
                //for (char c : strings.front().second) {
                    //text_shader->uniform<float>("character_number", 0.0f);
                    mesh.render();
                //}

                strings.pop();
            }
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