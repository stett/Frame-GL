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

            shader = new Shader(
                "Debug Shader",
                Shader::Preset::vert_standard(),
                Shader::Preset::frag_normals());
        }

        void teardown() {
            delete shader;
        }

        void step(float dt) {

            Camera* camera = render->display_camera();
            camera->bind_target();
            shader->bind();

            // Grab camera transform data
            shader->uniform(shader->uniforms().view, camera->view_matrix());
            shader->uniform(shader->uniforms().projection, camera->projection_matrix());

            // Set up GL

            // Draw debug elements
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glDisable(GL_CULL_FACE);
            glLineWidth(2.0f);
            render_lines();
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glEnable(GL_CULL_FACE);
            render_cubes();

            // Tear down
            shader->unbind();
            camera->unbind_target();
        }

    private:

        void render_lines() {

            // Just need one model matrix
            shader->uniform(shader->uniforms().model, glm::mat4(1.0f));

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
            mesh.render();
        }

        void render_cubes() {
            Resource<Mesh> mesh = Mesh::Factory::cube(1.0f);

            while (!cubes.empty()) {
                shader->uniform(shader->uniforms().model, cubes.front());
                mesh->render();
                cubes.pop();
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

    private:
        Render* render;
        Shader* shader;
        std::queue< std::pair< glm::vec3, glm::vec3 > > lines;
        std::queue< glm::mat4 > cubes;
    };
}