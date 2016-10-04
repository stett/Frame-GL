#pragma once
#include <string>
#include <vector>
#include <memory>
#include "frame/Resource.h"
#include "frame_gl/math.h"

namespace frame
{
    enum ShaderUniform {
        Model = 0,
        View,
        Projection
    };

    struct ShaderUniformLocations {
        int model;
        int view;
        int projection;
    };

    /// \class ShaderPart
    /// \brief One part of a full shader program.
    class ShaderPart {
    public:
        enum Type {
            Compute                 = 0x91B9, // GL_COMPUTE_SHADER
            Vertex                  = 0x8B31, // GL_VERTEX_SHADER
            TesselationControl      = 0x8E88, // GL_TESS_CONTROL_SHADER
            TesselationEvaluation   = 0x8E87, // GL_TESS_EVALUATION_SHADER
            Geometry                = 0x8DD9, // GL_GEOMETRY_SHADER
            Fragment                = 0x8B30, // GL_FRAGMENT_SHADER
        };

    public:
        ShaderPart(Type type, const std::string& source) : ShaderPart(type, std::vector<std::string>({ source })) {}
        ShaderPart(Type type, const std::vector<std::string>& sources);
        ~ShaderPart();
        ShaderPart(const ShaderPart& other) = delete;
        ShaderPart& operator=(const ShaderPart& other) = delete;
        unsigned int id() { return _id; }

    private:
        Type type;
        unsigned int _id;
    };

    /// \class Shader
    /// \brief A linked collection of ShaderPass objects
    class Shader {
    public:
        Shader();
        Shader(const std::string& name, const Resource<ShaderPart>& pass1);
        Shader(const std::string& name, const Resource<ShaderPart>& pass1, const Resource<ShaderPart>& pass2);
        Shader(const std::string& name, const Resource<ShaderPart>& pass1, const Resource<ShaderPart>& pass2, const Resource<ShaderPart>& pass3);
        Shader(const std::string& name, const std::vector< Resource<ShaderPart> >& passes);
        ~Shader();
        Shader(const Shader& other) = delete;
        Shader& operator=(const Shader& other) = delete;

    public:
        void bind();
        void unbind();
        static void unbind_all();

        const ShaderUniformLocations& uniforms() { return _uniforms; }

        template <typename T>
        void uniform(const char* name, const T& value)
        { uniform(locate(name), value); }

        void uniform(int location, int value);
        void uniform(int location, float value);
        void uniform(int location, const vec2& value);
        void uniform(int location, const vec3& value);
        void uniform(int location, const vec4& value);
        void uniform(int location, const mat4& value);
        void uniform(ShaderUniform location, const mat4& value);
        int locate(const char* name);

        unsigned int id() { return _id; }

    private:
        void compile();
        void link();
        const std::string& name() { return _name; }

    private:
        std::string _name;
        unsigned int _id;
        ShaderUniformLocations _uniforms;

    public:
        struct Preset {
            static Resource<ShaderPart> vert_standard();
            static Resource<ShaderPart> frag_uvs();
            static Resource<ShaderPart> frag_colors();
            static Resource<ShaderPart> frag_normals();
            static Resource<ShaderPart> frag_coords();
            static Resource<ShaderPart> frag_depth();
            static Resource<ShaderPart> frag_white();
            static Resource<Shader> model_uvs();
            static Resource<Shader> model_colors();
            static Resource<Shader> model_normals();
            static Resource<Shader> coords();
            static Resource<Shader> depth();

        private:
            Preset() {}
            ~Preset() {}
        };
    };
}