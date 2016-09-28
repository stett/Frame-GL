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

namespace
{
    struct TextLine {
        TextLine(const glm::vec3& position, const std::string& text, const glm::vec3& color, float size)
        : position(position), text(text), color(color), size(size) {}
        glm::vec3 position;
        std::string text;
        glm::vec3 color;
        float size;
    };
}

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
                    "uniform mat4 model;                                            "
                    "uniform mat4 view;                                             "
                    "uniform mat4 projection;                                       "
                    "out vec4 geom_position;                                        "
                    "void main() {                                                  "
                    "    mat4 transform = projection * view * model;                "
                    "    geom_position  = transform * vec4(vert_position, 1.0);     "
                    "    gl_Position    = geom_position;                            "
                    "}                                                              "
                ),

                Resource<ShaderPart>(ShaderPart::Type::Geometry,
                    "#version 330\n"
                    "layout(lines) in;"
                    "layout(line_strip, max_vertices = 16) out;"
                    "uniform float character_size;"
                    "uniform int character_number;"
                    "uniform int character_code;"
                    "in vec4 geom_position[2];"
                    "void main() {"
                    "   float scale = geom_position[0].w * character_size;"
                    "   vec4 pos = vec4((character_number) * scale, 0, 0, 0) + geom_position[0];"

                    // SPACE
                    "   if (character_code == 32) {" 
                    "   }"

                    // 0
                    "   else if (character_code == 48 || character_code == 48+32) {"
                    "       gl_Position = pos + vec4(.25, .1, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.25, .9, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.35, 1, 0, 0) * scale;     EmitVertex();"
                    "       gl_Position = pos + vec4(.65, 1, 0, 0) * scale;     EmitVertex();"
                    "       gl_Position = pos + vec4(.75, .9, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.75, .1, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.65, 0, 0, 0) * scale;     EmitVertex();"
                    "       gl_Position = pos + vec4(.35, 0, 0, 0) * scale;     EmitVertex();"
                    "       gl_Position = pos + vec4(.25, .1, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.3, .05, 0, 0) * scale;     EmitVertex();"
                    "       gl_Position = pos + vec4(.7, .95, 0, 0) * scale;    EmitVertex();"
                    "   }"

                    // 1
                    "   else if (character_code == 49 || character_code == 49+32) {"
                    "       gl_Position = pos + vec4(.35, 0, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.65, 0, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.5, 0, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.5, 1, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.4, .9, 0, 0) * scale;    EmitVertex();"
                    "   }"

                    // 2
                    "   else if (character_code == 50 || character_code == 50+32) {"
                    "       gl_Position = pos + vec4(.75, 0, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.25, 0, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.25, .25, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.65, .25, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.75, .35, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.75, .9, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.65, 1, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.35, 1, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.25, .9, 0, 0) * scale;    EmitVertex();"
                    "   }"

                    // 3
                    "   else if (character_code == 51 || character_code == 51+32) {"
                    "       gl_Position = pos + vec4(.25, 1, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.65, 1, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.75, .9, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.75, .6, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.65, .5, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.5, .5, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.65, .5, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.75, .4, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.75, .1, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.65, 0, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.25, 0, 0, 0) * scale;    EmitVertex();"
                    "   }"

                    // 4

                    // 5

                    // 6

                    // 7

                    // 8

                    // 9

                    // A
                    "   else if (character_code == 65 || character_code == 65+32) {"
                    "       gl_Position = pos + vec4(.25, 0, 0, 0) * scale;     EmitVertex();"
                    "       gl_Position = pos + vec4(.25, .9, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.35, 1, 0, 0) * scale;     EmitVertex();"
                    "       gl_Position = pos + vec4(.65, 1, 0, 0) * scale;     EmitVertex();"
                    "       gl_Position = pos + vec4(.75, .9, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.75, 0, 0, 0) * scale;     EmitVertex();"
                    "       gl_Position = pos + vec4(.75, .5, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.25, .5, 0, 0) * scale;    EmitVertex();"
                    "   }"

                    // B
                    "   else if (character_code == 66 || character_code == 66+32) {"
                    "       gl_Position = pos + vec4(.25, 0, 0, 0) * scale;     EmitVertex();"
                    "       gl_Position = pos + vec4(.25, 1, 0, 0) * scale;     EmitVertex();"
                    "       gl_Position = pos + vec4(.65, 1, 0, 0) * scale;     EmitVertex();"
                    "       gl_Position = pos + vec4(.75, .9, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.75, .5, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.25, .5, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.75, .5, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.75, .1, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.65, .0, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.25, .0, 0, 0) * scale;    EmitVertex();"
                    "   }"

                    // C
                    "   else if (character_code == 67 || character_code == 67+32) {"
                    "       gl_Position = pos + vec4(.75, 1, 0, 0) * scale;     EmitVertex();"
                    "       gl_Position = pos + vec4(.35, 1, 0, 0) * scale;     EmitVertex();"
                    "       gl_Position = pos + vec4(.25, .9, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.25, .1, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.35, 0, 0, 0) * scale;     EmitVertex();"
                    "       gl_Position = pos + vec4(.75, 0, 0, 0) * scale;     EmitVertex();"
                    "   }"

                    // D
                    "   else if (character_code == 68 || character_code == 68+32) {"
                    "       gl_Position = pos + vec4(.25, 0, 0, 0) * scale;     EmitVertex();"
                    "       gl_Position = pos + vec4(.25, 1, 0, 0) * scale;     EmitVertex();"
                    "       gl_Position = pos + vec4(.65, 1, 0, 0) * scale;     EmitVertex();"
                    "       gl_Position = pos + vec4(.75, .9, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.75, .1, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.65, 0, 0, 0) * scale;     EmitVertex();"
                    "       gl_Position = pos + vec4(.25, 0, 0, 0) * scale;     EmitVertex();"
                    "   }"

                    // E
                    "   else if (character_code == 69 || character_code == 69+32) {"
                    "       gl_Position = pos + vec4(.75, 0, 0, 0) * scale;     EmitVertex();"
                    "       gl_Position = pos + vec4(.25, 0, 0, 0) * scale;     EmitVertex();"
                    "       gl_Position = pos + vec4(.25, .5, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.75, .5, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.25, .5, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.25, 1, 0, 0) * scale;     EmitVertex();"
                    "       gl_Position = pos + vec4(.75, 1, 0, 0) * scale;     EmitVertex();"
                    "   }"

                    // F
                    "   else if (character_code == 70 || character_code == 70+32) {"
                    "       gl_Position = pos + vec4(.25, 0, 0, 0) * scale;     EmitVertex();"
                    "       gl_Position = pos + vec4(.25, .5, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.75, .5, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.25, .5, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.25, 1, 0, 0) * scale;     EmitVertex();"
                    "       gl_Position = pos + vec4(.75, 1, 0, 0) * scale;     EmitVertex();"
                    "   }"

                    // G
                    "   else if (character_code == 71 || character_code == 71+32) {"
                    "       gl_Position = pos + vec4(.75, 1, 0, 0) * scale;     EmitVertex();"
                    "       gl_Position = pos + vec4(.35, 1, 0, 0) * scale;     EmitVertex();"
                    "       gl_Position = pos + vec4(.25, .9, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.25, .1, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.35, 0, 0, 0) * scale;     EmitVertex();"
                    "       gl_Position = pos + vec4(.75, 0, 0, 0) * scale;     EmitVertex();"
                    "       gl_Position = pos + vec4(.75, .5, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.5, .5, 0, 0) * scale;     EmitVertex();"
                    "   }"

                    // H
                    "   else if (character_code == 72 || character_code == 72+32) {"
                    "       gl_Position = pos + vec4(.25, 0, 0, 0) * scale;     EmitVertex();"
                    "       gl_Position = pos + vec4(.25, 1, 0, 0) * scale;     EmitVertex();"
                    "       gl_Position = pos + vec4(.25, .5, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.75, .5, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.75, 0, 0, 0) * scale;     EmitVertex();"
                    "       gl_Position = pos + vec4(.75, 1, 0, 0) * scale;     EmitVertex();"
                    "   }"

                    // I
                    "   else if (character_code == 73 || character_code == 73+32) {"
                    "       gl_Position = pos + vec4(.35, 0, 0, 0) * scale;     EmitVertex();"
                    "       gl_Position = pos + vec4(.65, 0, 0, 0) * scale;     EmitVertex();"
                    "       gl_Position = pos + vec4(.5, 0, 0, 0) * scale;      EmitVertex();"
                    "       gl_Position = pos + vec4(.5, 1, 0, 0) * scale;      EmitVertex();"
                    "       gl_Position = pos + vec4(.35, 1, 0, 0) * scale;     EmitVertex();"
                    "       gl_Position = pos + vec4(.65, 1, 0, 0) * scale;     EmitVertex();"
                    "   }"

                    // J
                    "   else if (character_code == 74 || character_code == 74+32) {"
                    "       gl_Position = pos + vec4(.35, 1, 0, 0) * scale;     EmitVertex();"
                    "       gl_Position = pos + vec4(.65, 1, 0, 0) * scale;     EmitVertex();"
                    "       gl_Position = pos + vec4(.5, 1, 0, 0) * scale;      EmitVertex();"
                    "       gl_Position = pos + vec4(.5, .1, 0, 0) * scale;     EmitVertex();"
                    "       gl_Position = pos + vec4(.4, 0, 0, 0) * scale;      EmitVertex();"
                    "       gl_Position = pos + vec4(.25, 0, 0, 0) * scale;     EmitVertex();"
                    "   }"

                    // K
                    "   else if (character_code == 75 || character_code == 75+32) {"
                    "       gl_Position = pos + vec4(.25, 1, 0, 0) * scale;     EmitVertex();"
                    "       gl_Position = pos + vec4(.25, 0, 0, 0) * scale;     EmitVertex();"
                    "       gl_Position = pos + vec4(.25, .5, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.55, .5, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.65, .6, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.75, .7, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.75, 1, 0, 0) * scale;     EmitVertex();"
                    "       gl_Position = pos + vec4(.75, .7, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.65, .6, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.55, .5, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.65, .4, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.75, .3, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.75, 0, 0, 0) * scale;     EmitVertex();"
                    "   }"

                    // L
                    "   else if (character_code == 76 || character_code == 76+32) {"
                    "       gl_Position = pos + vec4(.75, 0, 0, 0) * scale;     EmitVertex();"
                    "       gl_Position = pos + vec4(.25, 0, 0, 0) * scale;     EmitVertex();"
                    "       gl_Position = pos + vec4(.25, 1, 0, 0) * scale;     EmitVertex();"
                    "   }"

                    // M
                    "   else if (character_code == 77 || character_code == 77+32) {"
                    "       gl_Position = pos + vec4(.25, 0, 0, 0) * scale;     EmitVertex();"
                    "       gl_Position = pos + vec4(.25, 1, 0, 0) * scale;     EmitVertex();"
                    "       gl_Position = pos + vec4(.4, 1, 0, 0) * scale;      EmitVertex();"
                    "       gl_Position = pos + vec4(.5, .9, 0, 0) * scale;     EmitVertex();"
                    "       gl_Position = pos + vec4(.5, .35, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.5, .9, 0, 0) * scale;     EmitVertex();"
                    "       gl_Position = pos + vec4(.6, 1, 0, 0) * scale;      EmitVertex();"
                    "       gl_Position = pos + vec4(.75, 1, 0, 0) * scale;     EmitVertex();"
                    "       gl_Position = pos + vec4(.75, 0, 0, 0) * scale;     EmitVertex();"
                    "   }"

                    // N
                    "   else if (character_code == 78 || character_code == 78+32) {"
                    "       gl_Position = pos + vec4(.25, 0, 0, 0) * scale;     EmitVertex();"
                    "       gl_Position = pos + vec4(.25, 1, 0, 0) * scale;     EmitVertex();"
                    "       gl_Position = pos + vec4(.75, 0, 0, 0) * scale;     EmitVertex();"
                    "       gl_Position = pos + vec4(.75, 1, 0, 0) * scale;     EmitVertex();"
                    "   }"

                    // O
                    "   else if (character_code == 79 || character_code == 79+32) {"
                    "       gl_Position = pos + vec4(.25, .1, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.25, .9, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.35, 1, 0, 0) * scale;     EmitVertex();"
                    "       gl_Position = pos + vec4(.65, 1, 0, 0) * scale;     EmitVertex();"
                    "       gl_Position = pos + vec4(.75, .9, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.75, .1, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.65, 0, 0, 0) * scale;     EmitVertex();"
                    "       gl_Position = pos + vec4(.35, 0, 0, 0) * scale;     EmitVertex();"
                    "       gl_Position = pos + vec4(.25, .1, 0, 0) * scale;    EmitVertex();"
                    "   }"

                    // P
                    "   else if (character_code == 80 || character_code == 80+32) {"
                    "       gl_Position = pos + vec4(.25, 0, 0, 0) * scale;     EmitVertex();"
                    "       gl_Position = pos + vec4(.25, 1, 0, 0) * scale;     EmitVertex();"
                    "       gl_Position = pos + vec4(.65, 1, 0, 0) * scale;     EmitVertex();"
                    "       gl_Position = pos + vec4(.75, .9, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.75, .6, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.65, .5, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.25, .5, 0, 0) * scale;    EmitVertex();"
                    "   }"

                    // Q
                    "   else if (character_code == 81 || character_code == 81+32) {"
                    "       gl_Position = pos + vec4(.65, 0, 0, 0) * scale;     EmitVertex();"
                    "       gl_Position = pos + vec4(.35, 0, 0, 0) * scale;     EmitVertex();"
                    "       gl_Position = pos + vec4(.25, .1, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.25, .9, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.35, 1, 0, 0) * scale;     EmitVertex();"
                    "       gl_Position = pos + vec4(.65, 1, 0, 0) * scale;     EmitVertex();"
                    "       gl_Position = pos + vec4(.75, .9, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.75, .1, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.65, 0, 0, 0) * scale;     EmitVertex();"
                    "       gl_Position = pos + vec4(.7, .05, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.75, 0, 0, 0) * scale;     EmitVertex();"
                    "       gl_Position = pos + vec4(.85, 0, 0, 0) * scale;     EmitVertex();"
                    "       gl_Position = pos + vec4(.95, .1, 0, 0) * scale;    EmitVertex();"
                    "   }"

                    // R
                    "   else if (character_code == 82 || character_code == 82+32) {"
                    "       gl_Position = pos + vec4(.25, 0, 0, 0) * scale;     EmitVertex();"
                    "       gl_Position = pos + vec4(.25, 1, 0, 0) * scale;     EmitVertex();"
                    "       gl_Position = pos + vec4(.65, 1, 0, 0) * scale;     EmitVertex();"
                    "       gl_Position = pos + vec4(.75, .9, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.75, .6, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.65, .5, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.25, .5, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.65, .5, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.75, .4, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.75, 0, 0, 0) * scale;     EmitVertex();"
                    "   }"

                    // S
                    "   else if (character_code == 83 || character_code == 83+32) {"
                    "       gl_Position = pos + vec4(.75, .9, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.65, 1, 0, 0) * scale;     EmitVertex();"
                    "       gl_Position = pos + vec4(.35, 1, 0, 0) * scale;     EmitVertex();"
                    "       gl_Position = pos + vec4(.25, .9, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.25, .6, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.35, .5, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.65, .5, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.75, .4, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.75, .1, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.65, 0, 0, 0) * scale;     EmitVertex();"
                    "       gl_Position = pos + vec4(.35, 0, 0, 0) * scale;     EmitVertex();"
                    "       gl_Position = pos + vec4(.25, .1, 0, 0) * scale;    EmitVertex();"
                    "   }"

                    // T
                    "   else if (character_code == 84 || character_code == 84+32) {"
                    "       gl_Position = pos + vec4(.5, 0, 0, 0) * scale;      EmitVertex();"
                    "       gl_Position = pos + vec4(.5, 1, 0, 0) * scale;      EmitVertex();"
                    "       gl_Position = pos + vec4(.25, 1, 0, 0) * scale;     EmitVertex();"
                    "       gl_Position = pos + vec4(.75, 1, 0, 0) * scale;     EmitVertex();"
                    "   }"

                    // U
                    "   else if (character_code == 85 || character_code == 85+32) {"
                    "       gl_Position = pos + vec4(.25, 1, 0, 0) * scale;     EmitVertex();"
                    "       gl_Position = pos + vec4(.25, .1, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.35, 0, 0, 0) * scale;     EmitVertex();"
                    "       gl_Position = pos + vec4(.65, 0, 0, 0) * scale;     EmitVertex();"
                    "       gl_Position = pos + vec4(.75, .1, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.75, 1, 0, 0) * scale;     EmitVertex();"
                    "   }"

                    // V
                    "   else if (character_code == 86 || character_code == 86+32) {"
                    "       gl_Position = pos + vec4(.25, 1, 0, 0) * scale;     EmitVertex();"
                    "       gl_Position = pos + vec4(.5, 0, 0, 0) * scale;      EmitVertex();"
                    "       gl_Position = pos + vec4(.75, 1, 0, 0) * scale;     EmitVertex();"
                    "   }"

                    // W
                    "   else if (character_code == 87 || character_code == 87+32) {"
                    "       gl_Position = pos + vec4(.25, 1, 0, 0) * scale;     EmitVertex();"
                    "       gl_Position = pos + vec4(.25, .1, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.35, 0, 0, 0) * scale;     EmitVertex();"
                    "       gl_Position = pos + vec4(.4, 0, 0, 0) * scale;      EmitVertex();"
                    "       gl_Position = pos + vec4(.5, .1, 0, 0) * scale;     EmitVertex();"
                    "       gl_Position = pos + vec4(.5, .5, 0, 0) * scale;     EmitVertex();"
                    "       gl_Position = pos + vec4(.5, .1, 0, 0) * scale;     EmitVertex();"
                    "       gl_Position = pos + vec4(.6, 0, 0, 0) * scale;      EmitVertex();"
                    "       gl_Position = pos + vec4(.65, 0, 0, 0) * scale;     EmitVertex();"
                    "       gl_Position = pos + vec4(.75, .1, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.75, 1, 0, 0) * scale;     EmitVertex();"
                    "   }"

                    // X
                    "   else if (character_code == 88 || character_code == 88+32) {"
                    "       gl_Position = pos + vec4(.25, 1, 0, 0) * scale;     EmitVertex();"
                    "       gl_Position = pos + vec4(.75, 0, 0, 0) * scale;     EmitVertex();"
                    "       gl_Position = pos + vec4(.5, .5, 0, 0) * scale;     EmitVertex();"
                    "       gl_Position = pos + vec4(.75, 1, 0, 0) * scale;     EmitVertex();"
                    "       gl_Position = pos + vec4(.25, 0, 0, 0) * scale;     EmitVertex();"
                    "   }"

                    // Y
                    "   else if (character_code == 89 || character_code == 89+32) {"
                    "       gl_Position = pos + vec4(.5, 0, 0, 0) * scale;      EmitVertex();"
                    "       gl_Position = pos + vec4(.5, .5, 0, 0) * scale;     EmitVertex();"
                    "       gl_Position = pos + vec4(.25, 1, 0, 0) * scale;     EmitVertex();"
                    "       gl_Position = pos + vec4(.5, .5, 0, 0) * scale;     EmitVertex();"
                    "       gl_Position = pos + vec4(.75, 1, 0, 0) * scale;     EmitVertex();"
                    "   }"

                    // Z
                    "   else if (character_code == 90 || character_code == 90+32) {"
                    "       gl_Position = pos + vec4(.25, 1, 0, 0) * scale;     EmitVertex();"
                    "       gl_Position = pos + vec4(.75, 1, 0, 0) * scale;     EmitVertex();"
                    "       gl_Position = pos + vec4(.25, 0, 0, 0) * scale;     EmitVertex();"
                    "       gl_Position = pos + vec4(.75, 0, 0, 0) * scale;     EmitVertex();"
                    "   }"

                    "   EndPrimitive();"
                    "}"
                ),

                Resource<ShaderPart>(ShaderPart::Type::Fragment,
                    "#version 330\n"
                    "uniform vec3 character_color;"
                    "out vec4 pixel_color;"
                    "void main() {"
                    "    pixel_color = vec4(character_color, 1);"
                    "}"
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
            shape_shader->uniform(ShaderUniform::View, camera->view_matrix());
            shape_shader->uniform(ShaderUniform::Projection, camera->projection_matrix());

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

            // Find the uniforms we'll be tweaking
            int character_color = text_shader->locate("character_color");
            int character_number = text_shader->locate("character_number");
            int character_code = text_shader->locate("character_code");
            int character_size = text_shader->locate("character_size");

            // Bind the text shader
            text_shader->bind();
            text_shader->uniform(ShaderUniform::View, camera->view_matrix());
            text_shader->uniform(ShaderUniform::Projection, camera->projection_matrix());

            Mesh mesh;
            mesh.add_position(vec3(0.0f));
            mesh.add_position(vec3(0.0f));
            mesh.add_line(0, 0);
            mesh.finalize();

            glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
            glDisable(GL_CULL_FACE);
            glLineWidth(1.0f);

            while (!strings.empty()) {

                const TextLine& line = strings.front();
                text_shader->uniform(ShaderUniform::Model, glm::translate(glm::mat4(1.0f), line.position));
                text_shader->uniform(character_size, line.size);
                text_shader->uniform(character_color, line.color);

                int i = 0;
                for (char c : line.text) {
                    text_shader->uniform(character_number, i++);
                    text_shader->uniform(character_code, (int)c);
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

        void text(const glm::vec3& position, const std::string& text, const glm::vec3& color=glm::vec3(1.0f), float size=0.025f) {
            strings.push(TextLine(position, text, color, size));
        }

    private:
        Render* render;
        Shader* shape_shader;
        Shader* text_shader;
        int text_shader_characters;
        std::queue< std::pair< glm::vec3, glm::vec3 > > lines;
        std::queue< glm::mat4 > cubes;
        std::queue< TextLine > strings;
    };
}