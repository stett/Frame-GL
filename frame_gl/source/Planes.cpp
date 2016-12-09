#include "frame/Frame.h"
#include "frame_gl/components/MeshRenderer.h"
#include "frame_gl/data/Shader.h"
#include "frame_gl/data/Texture.h"
#include "frame_gl/math.h"
#include "frame_gl/systems/Planes.h"
using namespace frame;

void Planes::setup() {

    auto plane_shader = Resource<Shader>(
        "Plane Shader",

        Resource<ShaderPart>(ShaderPart::Type::Vertex,
            "#version 330\n                                                 "
            "layout(location = 0)in vec3 vert_position;                     "
            "layout(location = 1)in vec3 vert_normal;                       "
            "layout(location = 2)in vec2 vert_uv;                           "
            "layout(location = 3)in vec4 vert_color;                        "
            "uniform mat4 model;                                            "
            "uniform mat4 view;                                             "
            "uniform mat4 projection;                                       "
            "out vec4 frag_position;                                        "
            "out vec4 frag_position_world;                                  "
            "out vec3 frag_normal;                                          "
            "out vec2 frag_uv;                                              "
            "out vec4 frag_color;                                           "
            "out float frag_depth;                                          "
            "void main() {                                                  "
            "   frag_position_world = model * vec4(10000 * vert_position, 1.0); "
            "   frag_position   = projection * view * frag_position_world;  "
            "   frag_normal     = vert_normal * inverse(mat3(model));       "
            "   frag_uv         = vert_uv;                                  "
            "   frag_color      = vert_color;                               "
            "   frag_depth      = frag_position.z / (2000.0);               "
            "   gl_Position     = frag_position;                            "
            "}                                                              "
        ),

        Resource<ShaderPart>(ShaderPart::Type::Fragment,
            "#version 330\n                             "
            "in vec2 frag_uv;                           "
            "in vec4 frag_position_world;               "
            "in float frag_depth;                       "
            "out vec4 pixel_color;                      "
            "void main() {                              "
            "   vec2 pos = frag_position_world.xz;"
            "   if (pos.x < 0) pos.x = 1-pos.x;"
            "   if (pos.y < 0) pos.y = 1-pos.y;"
            "   float gray = (int(pos.x) + int(pos.y)) % 2 == 0 ? 0.48 : 0.52;"
            "   pixel_color = vec4(vec3(gray), pow(1-frag_depth, 2));"
            "}                                          "
        )
    );

    Resource<Mesh> mesh(4, 2);
    vec3 normal(0.0f, 1.0f, 0.0f);

    mesh->set_vertices({
        vec3(-0.5f, 0.0f, -0.5f) * 0.5f, vec3(-0.5f, 0.0f,  0.5f) * 0.5f,
        vec3(0.5f, 0.0f,  0.5f) * 0.5f, vec3(0.5f, 0.0f, -0.5f) * 0.5f },
        { normal, normal, normal, normal },
        { vec2(0.0f, 0.0f), vec2(0.0f, 1.0f), vec2(1.0f, 1.0f), vec2(1.0f, 0.0f) });

    mesh->set_triangles({ ivec3(0, 1, 2), ivec3(0, 2, 3) });

    plane = frame()->entities().add();
    plane->add<Transform>();
    plane->add<MeshRenderer>(
        mesh,
        Resource<Texture>(ivec2(1)),
        plane_shader,
        MeshRenderer::PolyMode::Fill,
        true);
}

void Planes::teardown() {
    frame()->entities().remove(plane);
}