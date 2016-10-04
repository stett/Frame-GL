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
using namespace frame;

namespace
{
    struct Line {
        Line(const glm::vec3& a, const glm::vec3& b, const glm::vec3& color, float thickness = 1.0f)
        : a(a), b(b), color(color), thickness(thickness) {}
        glm::vec3 a, b, color;
        float thickness;
    };

    struct String {
        String(const glm::vec3& position, const std::string& text, const glm::vec3& color, float size)
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
        DebugDraw(int main_layer=0, int gui_layer=1)
        : main_layer(main_layer), gui_layer(gui_layer) {}
        ~DebugDraw() {}

    protected:
        void setup() {

            render = frame()->systems().get<Render>();

            shape_shader = new Shader(
                "Debug Shape Shader",
                Shader::Preset::vert_standard(),
                Shader::Preset::frag_normals());

            line_shader = new Shader(
                "Debug Line Shader",
                Shader::Preset::vert_standard(),
                Shader::Preset::frag_colors());

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

                    // .
                    "   if (character_code == 46) {"
                    "       gl_Position = pos + vec4(.45, .1, 0, 0) * scale; EmitVertex();"
                    "       gl_Position = pos + vec4(.55, .1, 0, 0) * scale; EmitVertex();"
                    "       gl_Position = pos + vec4(.55, 0, 0, 0) * scale; EmitVertex();"
                    "       gl_Position = pos + vec4(.45, 0, 0, 0) * scale; EmitVertex();"
                    "   }"

                    // 0
                    "   else if (character_code == 48) {"
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
                    "   else if (character_code == 49) {"
                    "       gl_Position = pos + vec4(.35, 0, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.65, 0, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.5, 0, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.5, 1, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.4, .9, 0, 0) * scale;    EmitVertex();"
                    "   }"

                    // 2
                    "   else if (character_code == 50) {"
                    "       gl_Position = pos + vec4(.75, 0, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.25, 0, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.65, .35, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.75, .45, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.75, .9, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.65, 1, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.35, 1, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.25, .9, 0, 0) * scale;    EmitVertex();"
                    "   }"

                    // 3
                    "   else if (character_code == 51) {"
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
                    "   else if (character_code == 52) {"
                    "       gl_Position = pos + vec4(.25, 1, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.25, .5, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.75, .5, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.75, 1, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.75, 0, 0, 0) * scale;    EmitVertex();"
                    "   }"

                    // 5
                    "   else if (character_code == 53) {"
                    "       gl_Position = pos + vec4(.75, 1, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.25, 1, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.25, .75, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.65, .65, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.75, .55, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.75, .1, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.65, 0, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.35, 0, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.25, .1, 0, 0) * scale;    EmitVertex();"
                    "   }"

                    // 6
                    "   else if (character_code == 54) {"
                    "       gl_Position = pos + vec4(.75, 1, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.25, 1, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.25, 0, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.75, 0, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.75, .4, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.25, .4, 0, 0) * scale;    EmitVertex();"
                    "   }"

                    // 7
                    "   else if (character_code == 55) {"
                    "       gl_Position = pos + vec4(.25, 1, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.75, 1, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.25, 0, 0, 0) * scale;    EmitVertex();"
                    "   }"

                    // 8
                    "   else if (character_code == 56) {"
                    "       gl_Position = pos + vec4(.25, .5, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.25, 1, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.75, 1, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.75, .5, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.25, .5, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.25, 0, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.75, 0, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.75, 1, 0, 0) * scale;    EmitVertex();"
                    "   }"

                    // 9
                    "   else if (character_code == 57) {"
                    "       gl_Position = pos + vec4(.75, 0, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.75, 1, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.25, 1, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.25, .6, 0, 0) * scale;    EmitVertex();"
                    "       gl_Position = pos + vec4(.75, .6, 0, 0) * scale;    EmitVertex();"
                    "   }"

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
            Camera* camera;

            // Get the main display camera
            if (camera = render->display_camera(main_layer)) {

                // Bind the render target
                camera->bind_target();

                // Draw shit
                render_lines(camera);
                render_cubes(camera);
                render_text(camera, world_strings);

                // Tear down
                camera->unbind_target();
            }

            // Get the GUI display camera
            if (camera = render->display_camera(gui_layer)) {

                // Bind the render target
                camera->bind_target();

                // Draw shit
                render_text(camera, screen_strings);

                // Tear down
                camera->unbind_target();
            }
        }

    private:

        void render_lines(Camera* camera) {

            if (lines.empty())
                return;

            // Bind the shape shader
            line_shader->bind();
            line_shader->uniform(ShaderUniform::View, camera->view_matrix());
            line_shader->uniform(ShaderUniform::Projection, camera->projection_matrix());

            // Just need one model matrix
            line_shader->uniform(ShaderUniform::Model, glm::mat4(1.0f));

            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glDisable(GL_CULL_FACE);
            glLineWidth(1.0f);

            // Build a mesh with a bunch of lines
            Mesh mesh;
            int mesh_indices = 0;
            while (!lines.empty()) {
                auto& line = lines.front();
                mesh.add_position(line.a);
                mesh.add_position(line.b);
                mesh.add_color(vec4(line.color, 1.0f));
                mesh.add_color(vec4(line.color, 1.0f));
                mesh.add_line(mesh_indices, mesh_indices+1);
                mesh_indices += 2;
                lines.pop();
            }

            // Draw it
            mesh.finalize();
            mesh.render();

            line_shader->unbind();
        }

        void render_cubes(Camera* camera) {

            if (cubes.empty())
                return;

            // Bind the shape shader
            shape_shader->bind();
            shape_shader->uniform(ShaderUniform::View, camera->view_matrix());
            shape_shader->uniform(ShaderUniform::Projection, camera->projection_matrix());

            Resource<Mesh> mesh = Mesh::Factory::cube(1.0f);

            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glEnable(GL_CULL_FACE);

            while (!cubes.empty()) {
                shape_shader->uniform(ShaderUniform::Model, cubes.front());
                mesh->render();
                cubes.pop();
            }

            shape_shader->unbind();
        }

        void render_text(Camera* camera, std::queue< String >& strings, glm::vec2 offset=glm::vec2(0.0f)) {

            if (strings.empty() && strings.empty())
                return;

            // Find the uniforms we'll be tweaking
            int character_color = text_shader->locate("character_color");
            int character_number = text_shader->locate("character_number");
            int character_code = text_shader->locate("character_code");
            int character_size = text_shader->locate("character_size");
            //int character_offset = text_shader->locate("character_offset");

            // Bind the text shader
            text_shader->bind();

            Mesh mesh;
            mesh.add_position(vec3(0.0f));
            mesh.add_position(vec3(0.0f));
            mesh.add_line(0, 0);
            mesh.finalize();

            glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
            glDisable(GL_CULL_FACE);
            glDisable(GL_DEPTH_TEST);
            glLineWidth(1.0f);

            // Draw each string
            text_shader->uniform(ShaderUniform::View, camera->view_matrix());
            text_shader->uniform(ShaderUniform::Projection, camera->projection_matrix());
            //text_shader->uniform(character_offset, offset);

            while (!strings.empty()) {
                const String& line = strings.front();
                text_shader->uniform(ShaderUniform::Model, glm::translate(glm::mat4(1.0f), line.position + vec3(offset, 0.0f)));
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

        void line(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& color=glm::vec3(1.0f), float thickness=1.0f) {
            lines.push(Line(p0, p1, color, thickness));
        }

        void cube(const glm::vec3& point, float scale=1.0f) {
            cube(glm::scale(glm::translate(glm::mat4(1.0f), point), glm::vec3(scale)));
        }

        void cube(const glm::mat4& transform) {
            cubes.push(transform);
        }

        void text(const glm::vec3& position, const std::string& text, const glm::vec3& color=glm::vec3(1.0f), float size=0.025f) {
            world_strings.push(String(position, text, color, size));
        }

        void text(const glm::vec2& position, const std::string& text, const glm::vec3& color = glm::vec3(1.0f), float size = 0.025f) {
            screen_strings.push(String(glm::vec3(position.x, 0.0f, position.y), text, color, size));
        }

    private:
        Render* render;
        Shader* line_shader;
        Shader* shape_shader;
        Shader* text_shader;
        int text_shader_characters;
        std::queue< Line > lines;
        std::queue< glm::mat4 > cubes;
        std::queue< String > world_strings;
        std::queue< String > screen_strings;
        int main_layer;
        int gui_layer;
    };
}