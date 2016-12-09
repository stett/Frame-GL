#pragma once
#include <vector>
#include <unordered_map>
#include <memory>
#include <iterator>
#include "frame/Resource.h"
#include "frame_gl/math.h"

namespace frame
{

    struct VertexAttribute {
        char* name;
        size_t size;
        bool dynamic;

        bool operator==(const VertexAttribute& other) const {
            if (size != other.size) return false;
            if (dynamic != other.dynamic) return false;
            return strcmp(name, other.name) == 0;
        }

        bool operator!=(const VertexAttribute& other) const {
            return !operator==(other);
        }
    };

    class VertexAttributeSet {
    public:
        VertexAttributeSet(std::initializer_list<VertexAttribute> attributes)
            : _size(0), _count(attributes.size()), _attributes(new VertexAttribute[_count]) {

            // Copy the attributes
            std::copy(attributes.begin(), attributes.end(), std::begin(attributes));

            // Measure the total vertex size
            for (size_t i = 0; i < _count; ++i)
                _size += _attributes[i].size;
        }

        ~VertexAttributeSet() { delete[] _attributes; }

        inline size_t size() const { return _size; }
        inline size_t count() const { return _count;  }

    public:
        inline const VertexAttribute& operator[](size_t i) const { return _attributes[i]; }

        bool operator==(const VertexAttributeSet& other) const {
            if (_count != other._count) return false;
            if (_size != other._size) return false;
            for (size_t i = 0; i < _count; ++i)
                if (_attributes[i] != other._attributes[i])
                    return false;
            return true;
        }

    private:
        size_t _size;
        size_t _count;
        VertexAttribute* _attributes;
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
        Mesh(size_t vertex_count = 0, size_t triangle_count = 0, bool dynamic_triangles = false) : Mesh(DEFAULT_VERTEX_ATTRIBUTES, vertex_count, triangle_count, dynamic_triangles) {}
        Mesh(VertexAttributeSet attriubtes, size_t vertex_count = 0, size_t triangle_count = 0, bool dynamic_triangles = false);
        ~Mesh();

    public:
        Mesh(const Mesh& other) = delete;
        Mesh& operator=(const Mesh& other) = delete;

    public:
        inline void render();
        inline void bind();
        inline void draw();
        inline void unbind();

    public:

        ///\brief Set values for a particular attribute for all vertices.
        ///       If count==0, then the vertex buffers will not be resized.
        template <typename T>
        void set_vertices(const char* attribute_name, const T* values, size_t count=0) {
            int index = find_attribute_index(attribute_name);
            if (index != -1) set_vertices<T>(index, values, count);
        }

        ///\brief Set values for a particular attribute for all vertices
        template <typename T>
        void set_vertices(int attribute_index, const T* values, size_t count=0) {
            assert(sizeof(T) == _attributes[attribute_index].size);
            if (count) set_vertex_count(count);
            size_t size = _vertex_count * _attributes[attribute_index].size;
            memcpy(buffers[attribute_index].data, values, size);
        }

        template <typename... T>
        void set_vertices(std::initializer_list<T>... values) {
            set_vertices<T>(0, values...);
        }

        template <typename T0, typename... T>
        void set_vertices(size_t first_attribute_index, std::initializer_list<T0> values0, std::initializer_list<T>... values) {
            // Note: The reason we've shaved off T0 is to extract the size of the initializer list.
            // Each list should have the same length or else there's a problem... so be fucking careful.
            size_t count = values0.size();
            assert(count == values.size())...;
            size_t i = first_attribute_index;
            set_vertices(i++, values0, count);
            set_vertices(i++, values, count)...;
        }

        template <typename T>
        void set_vertex(size_t vertex_index, size_t attribute_index, const T& value) {
            assert(sizeof(T) == _attributes[attribute_index].size);
            memcpy(buffers[attribute_index].data, &value, sizeof(T));
        }

        template <typename... T>
        void set_vertex(size_t vertex_index, size_t first_attribute_index, const T&... values) {
            size_t i = first_attribute_index;
            set_vertex(vertex_index, i++, values.size())...;
        }

        template <typename... T>
        void set_vertex(size_t vertex_index, const T&... values) {
            set_vertex(vertex_index, 0, values...);
        }

        ///\brief Set a single vertex attribute value
        /*
        template <typename T>
        void set_vertex(const char* attribute_name, size_t vertex_index, const T& value) {
            int index = find_attribute_index(attribute_name);
            if (index != -1) set_vertex(index, vertex_index, value);
        }

        template <typename T>
        void set_vertex(size_t attribute_index, size_t vertex_index, const T& value) {
            size_t size = _attributes[attribute_index].size;
            assert(sizeof(T) == size);
            memcpy(buffers[attribute_index].data + vertex_index, &value, size);
        }
        */

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
            for (size_t i = 0; i < _attributes.count(); ++i)
                if (strcmp(_attributes[i].name, name) == 0)
                    return i;
            return -1;
        }

        void set_triangle(size_t triangle_index, const ivec3& triangle) {
            _triangles[triangle_index] = triangle;
        }

        void set_triangles(std::initializer_list<ivec3> triangles) {
            set_triangle_count(triangles.size());
            std::copy(triangles.begin(), triangles.end(), _triangles);
        }

        void set_triangles(const ivec3* triangles, size_t count) {
            set_triangle_count(count);
            memcpy(_triangles, triangles, count * sizeof(ivec3));
        }

        void append(const Mesh& other);

    public:
        inline int vertex_array_vao() const { return vao; }
        inline int index_buffer_vbo() const { return vbo_triangles; }

    public:
        inline size_t vertex_count() const { return _vertex_count; }
        inline size_t vertex_size() const { return  _attributes.size(); }
        inline const VertexAttributeSet& attributes() const { return _attributes; }
        inline bool dynamic_triangles() const { return _dynamic_triangles; }

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
        bool _dynamic_triangles;
        size_t block_size;
        char* block;
        unsigned int vao;
        unsigned int vbo_triangles;
        VertexBuffer* buffers;
        ivec3* _triangles;
    };
}