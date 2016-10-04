#pragma once
#include <vector>
#include <unordered_map>
#include "frame/Resource.h"
#include "frame_gl/math.h"

namespace frame
{
    /// \class Mesh
    /// \brief Representation and handle for creation and managing of a vertex buffer
    class Mesh {
    public:
        Mesh();
        ~Mesh();
        Mesh(const Mesh& other) = delete;
        Mesh& operator=(const Mesh& other) = delete;

    public:
        void render();
        void render_lines();
        void clear();
        void add_position(const vec3& position);
        void add_uv(const vec2& uv);
        void add_normal(const vec3& normal);
        void add_color(const vec4& color);
        void add_vertex(const vec3& position, const vec3& normal=vec3(0.0f), const vec2& uv=vec2(0.0f), const vec4& color=vec4(1.0f));
        void add_triangle(const ivec3& indices);
        void add_triangle(int a, int b, int c) { add_triangle(ivec3(a, b, c)); }
        void add_quad(const ivec4& indices);
        void add_quad(int a, int b, int c, int d) { add_quad(ivec4(a, b, c, d)); }
        void add_line(const ivec2& indices);
        void add_line(int a, int b) { add_line(ivec2(a, b)); }
        std::size_t num_vertices() { return positions.size(); }
        void load_obj_str(const std::string& obj, bool normalize=false, bool center=false);
        void finalize();

    private:
        void release_gl_objects();

    private:
        std::vector<vec3> positions;
        std::vector<vec3> normals;
        std::vector<vec2> uvs;
        std::vector<vec4> colors;
        std::vector<ivec3> triangles;
        std::vector<ivec2> lines;
        unsigned int vao;
        unsigned int vbo_positions;
        unsigned int vbo_normals;
        unsigned int vbo_uvs;
        unsigned int vbo_colors;
        unsigned int vbo_indices;

    public:

        /// \struct Factory
        /// \brief Container for static mesh factories
        struct Factory {
        private:
            Factory() {}
            ~Factory() {}

        public:
            static Resource<Mesh> load(const std::string& filename, bool normalize=false, bool center=false);
            static Resource<Mesh> combine(const std::vector< Resource<Mesh> >& meshes);
            static Resource<Mesh> rectangle(const vec2& size=vec2(1.0f), const vec3& center=vec3(0.0f));
            static Resource<Mesh> circle(float radius=0.5f, const vec3& center=vec3(0.0f), float verts_per_length=1.0f);
            static Resource<Mesh> arrow(const vec3& base, const vec3& tip, const vec4& color=vec4(1.0f), float size=1.0f);
            static Resource<Mesh> quad(const vec2& size=vec2(1.0f), const vec3& normal=vec3(0.0f, 0.0f, 1.0f), const vec4& color=vec4(1.0f));
            static Resource<Mesh> cube(float edge=1.0f, const vec4& color=vec4(1.0f), bool smooth=false);
            static Resource<Mesh> sphere(float radius=0.5f, int recursion=0, const vec4& color=vec4(1.0f));

        private:
            static int get_midpoint(std::unordered_map<ivec2, int>& cache, std::vector<vec3>& vertices, ivec2 indices);
        };
    };
}