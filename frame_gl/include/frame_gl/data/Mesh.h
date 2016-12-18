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
        std::string name;
        size_t size;
        bool dynamic;

        bool operator==(const VertexAttribute& other) const {
            if (size != other.size) return false;
            if (dynamic != other.dynamic) return false;
            return name == other.name;
        }

        bool operator!=(const VertexAttribute& other) const {
            return !operator==(other);
        }
    };

    class VertexAttributeSet {
    public:
        VertexAttributeSet(std::initializer_list<VertexAttribute> attributes)
            : _size(0), _count(attributes.size()), _attributes(new VertexAttribute[_count]) {

            // Copy the attributes & measure total vertex size
            size_t i = 0;
            for (auto attribute : attributes) {
                _attributes[i++] = attribute;
                _size += attribute.size;
            }
        }

        VertexAttributeSet(const VertexAttributeSet& other)
            : _size(other._size), _count(other._count), _attributes(new VertexAttribute[_count]) {

            // Copy the attributes
            for (size_t i = 0; i < _count; ++i)
                _attributes[i] = other._attributes[i];
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
        void render();
        void draw();
        void bind();
        void unbind();

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
            #ifdef FRAME_ASSERTS
            assert(sizeof(T) == _attributes[attribute_index].size);
            #endif
            if (count) set_vertex_count(count);
            size_t size = _vertex_count * _attributes[attribute_index].size;
            memcpy(buffers[attribute_index].data, values, size);
        }

        template <typename T>
        void set_vertices(size_t attribute_index, std::initializer_list<T> values) {
            size_t size = _attributes[attribute_index].size;
            #ifdef FRAME_ASSERTS
            assert(sizeof(T) == size);
            #endif
            set_vertex_count(values.size());

            // Copy the initializer list data
            // TODO: Can we make this a memcpy? It'd be so nice.
            T* buffer = (T*)(buffers[attribute_index].data);
            size_t i = 0;
            for (auto& value : values)
                buffer[i++] = value;

            // SAFE? NO?? Apparently, no.
            //memcpy(buffers[attribute_index].data, values.begin(), size);

            // Update the gfx buffer
            update_vertex_buffer(attribute_index, 0, _vertex_count);
        }

        template <typename... T>
        void set_vertices(std::initializer_list<T>... values) {
            set_vertices(0, values...);
        }

        // TODO: Name these functions something more fitting... also hide them.
        template <typename T0, typename T1, typename... T>
        void check_vertices(std::initializer_list<T0> values0, std::initializer_list<T1> values1, std::initializer_list<T>... values) {
            check_vertices(values0, values1);
            check_vertices(values1, values...);
        }

        template <typename T0, typename T1>
        void check_vertices(std::initializer_list<T0> values0, std::initializer_list<T1> values1) {
            #ifdef FRAME_ASSERTS
            assert(values0.size() == values1.size());
            #endif
        }

        template <typename T0, typename... T>
        void set_vertices(size_t first_attribute_index, std::initializer_list<T0> values0, std::initializer_list<T>... values) {
            // Note: Each list should have the same length or else there's a problem... so be fucking careful.

            #ifdef FRAME_ASSERTS
            check_vertices(values0, values...);
            #endif

            set_vertices(first_attribute_index, values0);
            set_vertices(first_attribute_index+1, values...);
        }

        template <typename T>
        void set_vertex_attribute(size_t vertex_index, size_t attribute_index, const T& value) {
            #ifdef FRAME_ASSERTS
            assert(sizeof(T) == _attributes[attribute_index].size);
            #endif
            memcpy(buffers[attribute_index].data, &value, sizeof(T));
        }

        template <typename T0, typename... T>
        void set_vertex_attributes(size_t vertex_index, size_t first_attribute_index, const T0& value0, const T&... values) {
            set_vertex(vertex_index, first_attribute_index, value0);
            set_vertex(vertex_index, first_attribute_index+1, values...);
        }

        template <typename... T>
        void set_vertex(size_t vertex_index, const T&... values) {
            set_vertex_attributes(vertex_index, 0, values...);
        }

        template <typename T>
        T* get_vertices(const char* name) {
            return get_vertices(find_attribute_index(name));
        }

        template <typename T>
        T* get_vertices(size_t attribute_index) {
            size_t size = _attributes[attribute_index].size;
            #ifdef FRAME_ASSERTS
            assert(sizeof(T) == size);
            #endif
            return buffers[attribute_index].data;
        }

        int find_attribute_index(const char* name) {
            for (size_t i = 0; i < _attributes.count(); ++i)
                if (strcmp(_attributes[i].name.c_str(), name) == 0)
                    return i;
            return -1;
        }

        void set_triangle(size_t triangle_index, const ivec3& triangle) {
            _triangles[triangle_index] = triangle;
            update_index_buffer(triangle_index);
        }

        void set_triangles(std::initializer_list<ivec3> triangles) {
            set_triangle_count(triangles.size());
            memcpy(_triangles, triangles.begin(), sizeof(ivec3) * triangles.size());
            update_index_buffer();
        }

        void set_triangles(const ivec3* triangles, size_t count) {
            set_triangle_count(count);
            memcpy(_triangles, triangles, count * sizeof(ivec3));
            update_index_buffer();
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
        void update_vertex_buffers();                               ///< Send all vertex data from local buffer to gfx
        void update_vertex_buffers(size_t i);                       ///< Send a single vertex from local buffer to gfx
        void update_vertex_buffers(size_t i0, size_t i1);           ///< Send a range of vertices to gfx
        void update_vertex_buffer(size_t i, size_t i0, size_t i1); ///< Send a range of vertices from a single buffer to gfx
        void update_index_buffer();                                 ///< Update all triangles
        void update_index_buffer(size_t i);                         ///< Update a single triangle
        void update_index_buffer(size_t i0, size_t i1);             ///< Update a range of triangles

    private:
        void resize_block(size_t vertex_count, size_t triangle_count);
        void create_buffers();  ///< Create vertex and array buffers
        void destroy_buffers(); ///< Destroy vertex and array buffers

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