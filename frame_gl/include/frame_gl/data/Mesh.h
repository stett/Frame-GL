#pragma once
#include <vector>
#include <unordered_map>
#include <memory>
#include "frame/Resource.h"
#include "frame_gl/math.h"

namespace frame
{

    struct VertexAttribute {
        char* name;
        size_t size;
        bool dynamic;
    };

    class VertexAttributeSet {
    public:
        VertexAttributeSet(std::initializer_list<VertexAttribute> attributes)
            : _size(0), _count(attributes.size()), attributes(new VertexAttribute[_count]) {

            // Copy the attributes
            std::copy(attributes.begin(), attributes.end(), std::begin(attributes));

            // Measure the total vertex size
            for (int i = 0; i < attributes.count(); ++i)
                size += attributes[i].size;
        }

        ~VertexAttributeSet() { delete[] attributes; }

        inline const VertexAttribute& operator[](size_t i) const { return attributes[i]; }
        inline size_t size() const { return _size; }
        inline size_t count() const { return _count;  }

    private:
        size_t _size;
        size_t _count;
        VertexAttribute* attributes;
    };

    struct VertexBuffer {
        char* data;
        unsigned int vbo;
        size_t size;
    };

    const VertexAttributeSet DEFAULT_VERTEX_ATTRIBUTES =
    {
        {"position", sizeof(vec3), false},
        {"normal", sizeof(vec3), false},
        {"uv", sizeof(vec2), false},
        {"color", sizeof(vec4), false},
        {"weight-indexes", sizeof(vec4), false},
        {"weight-offset-0", sizeof(vec4), false},
        {"weight-offset-1", sizeof(vec4), false},
        {"weight-offset-2", sizeof(vec4), false},
        {"weight-offset-3", sizeof(vec4), false},
    };

    /// \class Mesh
    /// \brief Representation and handle for creation and managing of a vertex buffer
    class Mesh {

    public:
        Mesh(VertexAttributeSet attriubtes=DEFAULT_VERTEX_ATTRIBUTES, size_t vertex_count=0, size_t triangle_count=0);
        ~Mesh();

    public:
        Mesh(const Mesh& other) = delete;
        Mesh& operator=(const Mesh& other) = delete;

    public:
        void render();

    public:

        ///\brief Set values for a particular attribute for all vertices
        template <typename T>
        void set_vertices(const char* attribute_name, const T* values) {
            int index = find_attribute_index(attribute_name);
            if (index != -1)
                set_vertices<T>(index, values);
        }

        ///\brief Set values for a particular attribute for all vertices
        template <typename T>
        void set_vertices(int attribute_index, const T* values) {
            size_t size = _vertex_count * _attributes[attribute_index].size;
            memcpy(buffers[attribute_index].data, values, size);
        }

        ///\brief Set a single vertex attribute value
        template <typename T>
        void set_vertex(const char* attribute_name, size_t vertex_index, const T& value) {
            int index = find_attribute_index(attribute_name);
            if (index != -1)
                set_vertex(index, vertex_index, value);
        }

        template <typename T>
        void set_vertex(size_t attribute_index, size_t vertex_index, const T& value) {
            size_t size = _attributes[attribute_index].size;
            assert(sizeof(T) == size);
            memcpy(buffers[attribute_index].data + vertex_index, &value, size);
        }


        template <typename... T>
        void set_vertex(size_t vertex_index, const T&... values) {
            size_t i = 0;
            set_vertex(i++, vertex_index, value)...;
        }

        template <typename T>
        T* get_vertices(const char* name) {
            return get_vertices(find_attribute_index(name));
        }

        template <typename T>
        T* get_vertices(size_t attribute_index) {
            size_t size = _attributes[attribute_index].size;
            assert(sizeof(T) == size);
            return buffers[attribute_index].data;
        }

        int find_attribute_index(const char* name) {
            for (int i = 0; i < _attributes.count(); ++i)
                if (strcmp(_attributes[i].name, name) == 0)
                    return i;
            return -1;
        }

        void set_positions(const vec3* positions)
        { set_vertices<vec3>("position", positions); }

        /*
        void set_normals(const vec3* normals);
        void set_uvs(const vec2* uvs);
        void set_colors(const vec4* colors);
        void set_weights(const ivec4* indices, const vec4** offsets);
        void set_triangles(const ivec3* triangles);
        void set_lines(const ivec2* lines);
        */

        /*
        void set_position(size_t i, const vec3& position)
        { set_vertex<vec3>("position", i, position); }

        void set_uv(size_t i, const vec3& position)
        { set_uv<vec3>("uv", i, position); }

        void set_uv(size_t i, const vec2& uv);
        void set_normal(size_t i, const vec3& normal);
        void set_color(size_t i, const vec4& color);
        void set_weight(size_t i, const ivec4& indices, const vec4(&offsets)[4]);
        void set_vertex(size_t i, const vec3& position, const vec3& normal=vec3(0.0f), const vec2& uv=vec2(0.0f), const vec4& color=vec4(1.0f));
        void set_triangle(size_t i, const ivec3& indices);
        void set_triangle(size_t i, int a, int b, int c) { add_triangle(ivec3(a, b, c)); }
        void set_quad(size_t i, const ivec4& indices);
        void set_quad(size_t i, int a, int b, int c, int d) { add_quad(ivec4(a, b, c, d)); }
        void set_line(size_t i, const ivec2& indices);
        void set_line(size_t i, int a, int b) { add_line(ivec2(a, b)); }
        */

        void load_obj_str(const std::string& obj, bool normalize=false, bool center=false);

    public:
        size_t vertex_count() const { return _vertex_count; }
        size_t vertex_size() const { return  _attributes.size(); }
        const VertexAttributeSet& attributes() const { return _attributes; }

    public:
        void resize(size_t vertex_count, size_t triangle_count);
        void set_vertex_count(size_t vertex_count);
        void set_triangle_count(size_t triangle_count);

    public:
        void update_buffers();                      ///< Send all vertex data from local buffer to gfx
        void update_buffers(size_t i);              ///< Send a single vertex from local buffer to gfx
        void update_buffers(size_t i0, size_t i1);  ///< Send a range of vertices to gfx

    private:
        void resize_block(size_t vertex_count, size_t triangle_count);
        void create_buffers();                      ///< Create vertex and array buffers
        void destroy_buffers();                     ///< Destroy vertex and array buffers

    private:
        VertexAttributeSet _attributes;
        size_t _vertex_count;
        size_t _triangle_count;
        size_t block_size;
        char* block;
        unsigned int vao;
        unsigned int vbo_triangles;
        VertexBuffer* buffers;
        ivec3* triangles;
    };
}