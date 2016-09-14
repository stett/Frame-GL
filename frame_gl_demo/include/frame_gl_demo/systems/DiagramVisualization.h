#pragma once
#include <unordered_map>
#include "frame/System.h"
#include "frame/Diagram.h"
#include "frame/Frame.h"
#include "frame_gl/data/Mesh.h"
#include "frame_gl/data/Shader.h"
#include "frame_gl/data/Texture.h"
#include "frame_gl/components/MeshRenderer.h"
#include "frame_gl/data/Mesh.h"
#include "frame_gl/data/Shader.h"
#include "frame_gl_demo/components/Draggable.h"
using namespace frame;

namespace frame_demo
{
    /// \DiagramNodeComponent
    ///
    FRAME_COMPONENT(DiagramNodeComponent) {
    public:
        DiagramNodeComponent() : _node(nullptr) {}
        DiagramNodeComponent(DiagramNode* node) : _node(node) {}
        ~DiagramNodeComponent() {}

    public:
        const DiagramNode* node() const { return _node; }

    private:
        DiagramNode* _node;
    };


    /// \DiagramVisualization
    ///
    FRAME_SYSTEM(DiagramVisualization, Node<DiagramNodeComponent, Transform>) {
    public:
        DiagramVisualization() : diagram(&scratch), spline(nullptr) {}
        DiagramVisualization(Diagram* diagram) : diagram(diagram), spline(nullptr) {}
        ~DiagramVisualization() {}

    protected:
        void setup() {

            node_mesh = Mesh::Factory::quad(vec2(1.0f), vec3(0.0f, 0.0f, 1.0f), vec4(vec3(0.25f), 1.0f));
            node_texture = Resource<Texture>(ivec2(1));
            node_shader = Shader::Preset::model_colors();

            spline_shader = Resource<Shader>(
                "Spline Shader",

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

                // TODO: Factorize polynomials
                Resource<ShaderPart>(ShaderPart::Type::Geometry,
                    "#version 330\n                                 "
                    "layout(lines) in;                              "
                    "layout(triangle_strip, max_vertices = 128) out;"
                    "uniform mat4 model;                            "
                    "uniform mat4 view;                             "
                    "uniform mat4 projection;                       "
                    "in vec3 geom_position[];                       "
                    "in vec3 geom_normal[];                         "
                    "out vec3 frag_color;                           "
                    "                                               "
                    "float h00(float t) {                           "
                    "   return (2 * t * t * t) - (3 * t * t) + 1;   "
                    "}                                              "
                    "float h10(float t) {                           "
                    "   return (t * t * t) - (2 * t * t) + t;       "
                    "}                                              "
                    "float h01(float t) {                           "
                    "   return  - (2 * t * t * t) + (3 * t * t);    "
                    "}                                              "
                    "float h11(float t) {                           "
                    "   return (t * t * t) - (t * t);               "
                    "}                                              "
                    "                                               "
                    "void main() {                                  "
                    "   mat4 transform = projection * view * model; "
                    "   float coef = 1.0f / 63.0f;                  "
                    "   for (int i = 0; i < 64; ++i) {              "
                    "       float t = 1 - i * coef;                 "
                    "       vec3 position =                         "
                    "           geom_position[0] * h00(t) +         "
                    "           geom_position[1] * h01(t) +         "
                    "           geom_normal[0]   * h10(t) -         "
                    "           geom_normal[1]   * h11(t);          "
                    "       frag_color = vec3(1.0f, t, 0.25f);      "
                    "       gl_Position = transform * vec4(position - vec3(0, 0.05, 0), 1);"
                    "       EmitVertex();                           "
                    "       gl_Position = transform * vec4(position + vec3(0, 0.05, 0), 1);"
                    "       EmitVertex();                           "
                    "   }                                           "
                    "   EndPrimitive();                             "
                    "}                                              "
                ),

                Resource<ShaderPart>(ShaderPart::Type::Fragment,
                    "#version 330\n                                 "
                    "in vec3 frag_color;                            "
                    "out vec4 pixel_color;                          "
                    "void main() {                                  "
                    "   pixel_color = vec4(frag_color, 1);          "
                    "}                                              "
                )
            );

            rebuild();
        }

    public:
        void set(Diagram& new_diagram) {
            scratch = Diagram();
            diagram = &new_diagram;
            rebuild();
        }

        void rebuild() {

            // Destroy all existing node objects
            for (auto e : node<DiagramNodeComponent, Transform>())
                frame()->entities().remove(e);

            // Clear the spline mesh verts & remove the old spline renderer
            if (spline) frame()->entities().remove(spline);
            spline_mesh->clear();
            std::size_t spline_mesh_indices = 0;

            // Create new ones
            vec3 pos(-3.0f, 3.0f, 0.0f);
            std::unordered_map<const DiagramNode*, vec3> node_positions;
            for (DiagramNode* node : diagram->nodes()) {
                auto node_entity = frame()->entities().add();

                // Add the new node to the map
                node_entity->add<DiagramNodeComponent>(node);
                node_entity->add<Transform>()->set_translation(pos);
                node_entity->add<MeshRenderer>(node_mesh, node_texture, node_shader, MeshRenderer::PolyMode::Fill);
                node_entity->add<ClickableArea>();
                node_entity->add<Draggable>();

                // Save this position
                node_positions[node] = pos;

                // Increment the position for the next node
                pos.x += 2.0f;
                if (pos.x > 3.0f) {
                    pos.x = -3.0f;
                    pos.y -= 2.0f;
                }

                // Generate spline vertices
                for (DiagramInputSocket* input : node->inputs()) {
                    auto from = input->connection();
                    if (from == nullptr) continue;
                    auto pos0 = node_positions[from->node()];
                    auto pos1 = node_positions[node];
                    auto tan = (pos0 - pos1).length();

                    spline_mesh->add_position(pos0);
                    spline_mesh->add_normal(vec3(tan, 0.0f, 1.0f));

                    spline_mesh->add_position(pos1);
                    spline_mesh->add_normal(vec3(-tan, 0.0f, 1.0f));

                    spline_mesh->add_line(spline_mesh_indices, spline_mesh_indices+1);
                    //spline_mesh->add_triangle(spline_mesh_indices, spline_mesh_indices, spline_mesh_indices+1);
                    spline_mesh_indices += 2;
                }
            }

            // Create a renderer with the spline mesh
            spline = frame()->entities().add<Transform>();
            spline->add<MeshRenderer>(spline_mesh, node_texture, spline_shader, MeshRenderer::PolyMode::Fill);
        }

    private:
        Diagram scratch;
        Diagram* diagram;
        Entity* spline;
        Resource<Mesh>      node_mesh;
        Resource<Texture>   node_texture;
        Resource<Shader>    node_shader;
        Resource<Mesh>      spline_mesh;
        Resource<Shader>    spline_shader;
    };
}
