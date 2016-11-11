#define GLEW_STATIC
#include <string>
#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>
#include "frame/Log.h"
#include "frame/Resource.h"
#include "frame_gl/data/Shader.h"
#include "frame_gl/error.h"
using namespace frame;

ShaderPart::ShaderPart(Type type, const std::vector<std::string>& sources) : type(type) {

    // Convert shader sources to GL strings... :(
    std::vector<GLchar*> gl_sources;
    for (const std::string& source : sources)
        gl_sources.push_back((GLchar*)source.data());

    // Create and compile the shader
    _id = glCreateShader(type);
    glShaderSource(_id, sources.size(), gl_sources.data(), 0);
    glCompileShader(_id);

    // Print an error message if the compilation failed
    int status;
    glGetShaderiv(_id, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE) {
        int length;
        glGetShaderiv(_id, GL_INFO_LOG_LENGTH, &length);
        char* buffer = new char[length];
        glGetShaderInfoLog(_id, length, 0, buffer);
        Log::error("Shader part failed to compile:\n" + std::string(buffer));
        delete[] buffer;
        return;
    }

    // Success message
    Log::success("Shader part compiled successfully: " + std::to_string(_id));
}

ShaderPart::~ShaderPart() { glDeleteShader(_id); }

Shader::Shader() : _name("empty"), _id(0) {}
Shader::Shader(const std::string& name, const Resource<ShaderPart>& pass1) : Shader(name, std::vector< Resource<ShaderPart> >({ pass1 })) {}
Shader::Shader(const std::string& name, const Resource<ShaderPart>& pass1, const Resource<ShaderPart>& pass2) : Shader(name, std::vector< Resource<ShaderPart> >({ pass1, pass2 })) {}
Shader::Shader(const std::string& name, const Resource<ShaderPart>& pass1, const Resource<ShaderPart>& pass2, const Resource<ShaderPart>& pass3) : Shader(name, std::vector< Resource<ShaderPart> >({ pass1, pass2, pass3 })) {}
Shader::Shader(const std::string& name, const std::vector< Resource<ShaderPart> >& parts) : _name(name) {

    // Create the new program
    _id = glCreateProgram();

    // Attach all the shader passes to it
    for (auto part : parts)
        glAttachShader(_id, part->id());

    // Link the shader program
    glLinkProgram(_id);

    // Print an error message if there was a problem
    int status;
    glGetProgramiv(_id, GL_LINK_STATUS, &status);
    if (status != GL_TRUE) {
        int length;
        glGetProgramiv(_id, GL_INFO_LOG_LENGTH, &length);
        char* buffer = new char[length];
        glGetProgramInfoLog(_id, length, 0, buffer);
        Log::error("Shader program failed to link: " + name + "\n" + std::string(buffer));
        delete[] buffer;
        return;
    }

    // Use the program
    glUseProgram(_id);

    // Find common uniforms
    _uniforms.model         = glGetUniformLocation(_id, "model");
    _uniforms.view          = glGetUniformLocation(_id, "view");
    _uniforms.projection    = glGetUniformLocation(_id, "projection");
    _uniforms.diffuse       = glGetUniformLocation(_id, "diffuse");

    glUseProgram(0);

    gl_check();

    // Print success message
    Log::success("Shader program linked: " + name + " (" + std::to_string(_id) + ")");
}

Shader::~Shader() {
    glDeleteProgram(_id);
}

void Shader::bind() {
    glUseProgram(_id);
}

void Shader::unbind() {
    unbind_all();
}

void Shader::unbind_all() {
    glUseProgram(0);
}

void Shader::uniform(int location, int value) const {
    glUniform1i(location, value);
    gl_check();
}

void Shader::uniform(int location, float value) const {
    glUniform1f(location, value);
    gl_check();
}

void Shader::uniform(int location, const vec2& value) const {
    glUniform2fv(location, 1, glm::value_ptr(value));
    gl_check();
}

void Shader::uniform(int location, const vec3& value) const {
    glUniform3fv(location, 1, glm::value_ptr(value));
    gl_check();
}

void Shader::uniform(int location, const vec4& value) const {
    glUniform4fv(location, 1, glm::value_ptr(value));
    gl_check();
}

void Shader::uniform(int location, const mat4& value) const {
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
    gl_check();
}

void Shader::uniform(ShaderUniform location, const mat4& value) const {
    //
    // TODO: This is horrible. Stupid using both enums
    //       and nasty, reducible switch statements.
    //
    if      (location == Model)         uniform(uniforms().model,       value);
    else if (location == View)          uniform(uniforms().view,        value);
    else if (location == Projection)    uniform(uniforms().projection,  value);
}

void Shader::uniform(ShaderUniform location, int value) const {
    if      (location == Diffuse)       uniform(uniforms().diffuse,     value);
}

void Shader::uniform_array(int array_location, const mat4* values, int count, int count_location) const {
    glUniformMatrix4fv(array_location, count, GL_FALSE, glm::value_ptr(values[0]));
    if (count_location != -1) glUniform1i(count_location, count);
    gl_check();
}

int Shader::locate(const char* uniform_name) const {

    glUseProgram(_id);

    int location = glGetUniformLocation(_id, uniform_name);
    if (location == -1)
        Log::warning("Uniform \"" + std::string(uniform_name) + "\" not found in shader \"" + _name + "\"");

    return location;
}

Resource<ShaderPart> Shader::Preset::vert_standard() {
    static Resource<ShaderPart> part(ShaderPart::Type::Vertex,
        "#version 330\n                                                         "
        "layout(location = 0)in vec3 vert_position;                             "
        "layout(location = 1)in vec3 vert_normal;                               "
        "layout(location = 2)in vec2 vert_uv;                                   "
        "layout(location = 3)in vec4 vert_color;                                "
        "uniform mat4 model;                                                    "
        "uniform mat4 view;                                                     "
        "uniform mat4 projection;                                               "
        "out vec4 frag_position;                                                "
        "out vec4 frag_position_world;                                          "
        "out vec3 frag_normal;                                                  "
        "out vec2 frag_uv;                                                      "
        "out vec4 frag_color;                                                   "
        "void main() {                                                          "
        "    mat4 transform = projection * view;                                "
        "    frag_position_world = model * vec4(vert_position, 1);              "
        "    frag_position  = transform * frag_position_world;                  "
        "    frag_normal    = normalize(vert_normal * inverse(mat3(model)));    "
        "    frag_uv        = vert_uv;                                          "
        "    frag_color     = vert_color;                                       "
        "    gl_Position    = frag_position;                                    "
        "}                                                                      "
    );

    return part;
}

Resource<ShaderPart> Shader::Preset::vert_skinned() {
    static Resource<ShaderPart> part(ShaderPart::Type::Vertex,
        "#version 330\n                                                             "
        "layout(location = 1)in vec3 vert_normal;                                   "
        "layout(location = 2)in vec2 vert_uv;                                       "
        "layout(location = 3)in vec4 vert_color;                                    "
        "layout(location = 4)in vec4 vert_weight_indices;                          "
        "layout(location = 5)in vec4 vert_weight_offsets[4];                        "
        "uniform mat4 model;                                                        "
        "uniform mat4 view;                                                         "
        "uniform mat4 projection;                                                   "
        "uniform mat4 bone_transforms[128];                                         "
        "out vec4 frag_position;                                                    "
        "out vec3 frag_normal;                                                      "
        "out vec2 frag_uv;                                                          "
        "out vec4 frag_color;                                                       "

        "highp float rand(vec2 co)"
        "{"
        "   highp float a = 12.9898;"
        "   highp float b = 78.233;"
        "   highp float c = 43758.5453;"
        "   highp float dt= dot(co.xy ,vec2(a,b));"
        "   highp float sn= mod(dt,3.14);"
        "   return fract(sin(sn) * c);"
        "}"

        "vec3 rand_color(float index) {"
        "    return vec3(rand(vec2(index, 0)), rand(vec2(0, index+1)), rand(vec2(index+1, 0)));"
        "}"

        "void main() {                                                              "

        "   vec4 vert_position =                                                    "
        "       bone_transforms[int(vert_weight_indices[0])] * vert_weight_offsets[0] +  "
        "       bone_transforms[int(vert_weight_indices[1])] * vert_weight_offsets[1] +  "
        "       bone_transforms[int(vert_weight_indices[2])] * vert_weight_offsets[2] +  "
        "       bone_transforms[int(vert_weight_indices[3])] * vert_weight_offsets[3];   "

        "   mat4 transform  = projection * view * model;                            "
        "   frag_position   = transform * vert_position;                            "
        "   frag_normal     = normalize(vert_normal * inverse(mat3(model)));        "
        "   frag_uv         = vert_uv;                                              "

        "   frag_color = vec4("
        "       .25 * rand_color(vert_weight_indices[0]) +"
        "       .25 * rand_color(vert_weight_indices[1]) +"
        "       .25 * rand_color(vert_weight_indices[2]) +"
        "       .25 * rand_color(vert_weight_indices[3]), 1);"

        "   gl_Position     = frag_position;                                        "
        "}                                                                          "
    );

    return part;
}

Resource<ShaderPart> Shader::Preset::frag_uvs() {
    static Resource<ShaderPart> part(ShaderPart::Type::Fragment,
        "#version 330\n                             "
        "in vec2 frag_uv;                           "
        "out vec4 pixel_color;                      "
        "void main() {                              "
        "   pixel_color = vec4(frag_uv, .5, 1);     "
        "}                                          "
    );
    return part;
}

Resource<ShaderPart> Shader::Preset::frag_diffuse() {
    static Resource<ShaderPart> part(ShaderPart::Type::Fragment,
        "#version 330\n                                             "
        "in vec2 frag_uv;                                           "
        "uniform sampler2D diffuse;                                 "
        //"uniform int diffuse;                                 "
        "out vec4 pixel_color;                                      "
        "void main() {                                              "
        //"   pixel_color = vec4(frag_uv, .5, 1);                     "
        "   pixel_color = vec4(texture(diffuse, frag_uv).bgr, 1.0f);"
        //"   pixel_color = vec4(texture(diffuse, frag_uv).rgb, 1.0f);"
        "}                                                          "
    );
    return part;
}

Resource<ShaderPart> Shader::Preset::frag_colors() {
    static Resource<ShaderPart> part(ShaderPart::Type::Fragment,
        "#version 330\n                 "
        "in vec4 frag_color;            "
        "out vec4 pixel_color;          "
        "void main() {                  "
        "    pixel_color = frag_color;  "
        "}                              "
    );
    return part;
}

Resource<ShaderPart> Shader::Preset::frag_normals() {
    static Resource<ShaderPart> part(ShaderPart::Type::Fragment,
        "#version 330\n                                             "
        "in vec3 frag_normal;                                       "
        "out vec4 pixel_color;                                      "
        "void main() {                                              "
        "    pixel_color = vec4((vec3(1) + frag_normal) * 0.5, 1);  "
        "}                                                          "
    );
    return part;
}

Resource<ShaderPart> Shader::Preset::frag_coords() {
    static Resource<ShaderPart> part(ShaderPart::Type::Fragment,
        "#version 330\n                                 "
        "in vec4 frag_color;                            "
        "out vec4 pixel_color;                          "
        "void main() {                                  "
        "    pixel_color = vec4(gl_FragCoord.xyz, 1);   "
        "}                                              "
    );
    return part;
}

Resource<ShaderPart> Shader::Preset::frag_depth() {
    static Resource<ShaderPart> part(ShaderPart::Type::Fragment,
        "#version 330\n                 "
        "in vec4 frag_color;            "
        "out vec4 pixel_color;          "
        "void main() {                  "
        "    pixel_color = vec4(gl_FragCoord.x * gl_FragCoord.w, gl_FragCoord.y * gl_FragCoord.w, gl_FragCoord.z, 1);"
        "}                              "
    );
    return part;
}

Resource<ShaderPart> Shader::Preset::frag_white() {
    static Resource<ShaderPart> part(ShaderPart::Type::Fragment,
        "#version 330\n                 "
        "out vec4 pixel_color;          "
        "void main() {                  "
        "    pixel_color = vec4(1);"
        "}                              "
    );
    return part;
}

Resource<ShaderPart> Shader::Preset::frag_flat() {
    static Resource<ShaderPart> part(ShaderPart::Type::Fragment,
        "#version 330\n                                                 "
        "in vec3 frag_normal;                                           "
        "out vec4 pixel_color;                                          "
        "uniform vec3 light_normal;                                     "
        "uniform vec3 light_color;                                      "
        "void main() {                                                  "
        "   float intensity = max(0, dot(light_normal, frag_normal));   "
        "   pixel_color = vec4(intensity * light_color, 1);             "
        "}                                                              "
    );
    return part;
}

Resource<Shader> Shader::Preset::model_uvs() {
    static Resource<Shader> shader("Model UVs", vert_standard(), frag_uvs());
    return shader;
}

Resource<Shader> Shader::Preset::model_colors() {
    static Resource<Shader> shader("Model Colors", vert_standard(), frag_colors());
    return shader;
}

Resource<Shader> Shader::Preset::model_normals() {
    static Resource<Shader> shader("Model Normals", vert_standard(), frag_normals());
    return shader;
}

Resource<Shader> Shader::Preset::coords() {
    static Resource<Shader> shader("Frag Screen Coords", vert_standard(), frag_coords());
    return shader;
}

Resource<Shader> Shader::Preset::depth() {
    static Resource<Shader> shader("Depth", vert_standard(), frag_depth());
    return shader;
}