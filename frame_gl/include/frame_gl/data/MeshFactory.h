#pragma once
#include "frame_gl/data/Mesh.h"

namespace frame
{
    namespace MeshFactory
    {
        Resource<Mesh> load_obj_file(const std::string& filename, bool normalize=false, bool center=false);
        Resource<Mesh> load_obj_string(const std::string& obj, bool normalize=false, bool center=false);
        Resource<Mesh> combine(const std::vector< Resource<Mesh> >& meshes, bool dynamic_triangles=false);
        Resource<Mesh> rectangle(const vec2& size=vec2(1.0f), const vec3& center=vec3(0.0f));
        Resource<Mesh> circle(float radius=0.5f, const vec3& center=vec3(0.0f), float verts_per_length=1.0f);
        Resource<Mesh> arrow(const vec3& base, const vec3& tip, const vec4& color=vec4(1.0f), float size=1.0f);
        Resource<Mesh> quad(const vec2& size=vec2(1.0f), const vec3& normal=vec3(0.0f, 0.0f, 1.0f), const vec4& color=vec4(1.0f));
        Resource<Mesh> cube(float edge=1.0f, const vec4& color=vec4(1.0f), bool smooth=false);
        Resource<Mesh> sphere(float radius=0.5f, int recursion=0, const vec4& color=vec4(1.0f));
    }
}