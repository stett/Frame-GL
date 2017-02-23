#define GLEW_STATIC
#define _CRT_SECURE_NO_WARNINGS
#include <memory>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <utility>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "frame/Log.h"
#include "frame/Resource.h"
#include "frame_gl/data/Mesh.h"
#include "frame_gl/math.h"
#include "frame_gl/error.h"
using namespace frame;

VertexAttributeSet VertexAttributeSet::operator+(const VertexAttributeSet& other) const {
    std::vector<VertexAttribute> attributes;
    for (size_t i = 0; i < _count; ++i)
        attributes.push_back(_attributes[i]);
    for (size_t i = 0; i < other._count; ++i)
        attributes.push_back(other[i]);
    return VertexAttributeSet(attributes);
}

Mesh::Mesh(VertexAttributeSet attributes, size_t vertex_count, size_t triangle_count, bool dynamic_triangles) :
    _attributes(attributes), _dynamic_triangles(dynamic_triangles), vao(0), block(0), _finalized(false) {

    if (glfwGetCurrentContext() == 0)
        Log::error("Can't create a mesh outside of an OpenGL context!");

    // Set up empty buffers, & get space for them in gfx
    resize_block(vertex_count, triangle_count);
}

Mesh::~Mesh() {
    //free(block);
    delete[] block;
    destroy_buffers();  // Delete gfx buffers
}

void Mesh::render() {
    bind();
    draw();
    unbind();
}

void Mesh::draw() {
    glDrawElements(GL_TRIANGLES, 3 * _triangle_count, GL_UNSIGNED_INT, 0);
}

void Mesh::bind() {
    finalize();
    glBindVertexArray(vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_triangles);
}

void Mesh::unbind() {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Mesh::resize(size_t vertex_count, size_t triangle_count) {
    if (vertex_count == _vertex_count &&
        triangle_count == _triangle_count)
        return;
    resize_block(vertex_count, triangle_count);
}

void Mesh::finalize() {
    if (_finalized) return;
    _finalized = true;
    create_buffers();
}

void Mesh::unfinalize() {
    if (!_finalized) return;
    _finalized = false;
    destroy_buffers();
}

void Mesh::set_vertex_count(size_t vertex_count) {
    resize(vertex_count, _triangle_count);
}

void Mesh::set_triangle_count(size_t triangle_count) {
    resize(_vertex_count, triangle_count);
}

void Mesh::resize_block(size_t vertex_count, size_t triangle_count) {

    //
    // My my, isn't this a little beast.
    //

    // Compute new sizes;
    size_t buffers_size = _attributes.count() * sizeof(VertexBuffer);
    size_t vertex_size = vertex_count * _attributes.size();
    size_t triangle_size = triangle_count * sizeof(ivec3);

    // Allocate the new block
    char* new_block = new char[buffers_size + vertex_size + triangle_size];
    //char* new_block = static_cast<char*>(malloc(buffers_size + vertex_size + triangle_size));

    // Set basic array locations
    VertexBuffer* new_buffers = (VertexBuffer*)(new_block);
    ivec3* new_triangles = (ivec3*)(new_block + buffers_size + vertex_size);

    // Set up buffers
    char* location = new_block + buffers_size;
    for (size_t i = 0; i < _attributes.count(); ++i) {
        new_buffers[i].data = location;
        new_buffers[i].size = vertex_count * _attributes[i].size;
        location += vertex_count * _attributes[i].size;
    }

    if (block) {

        // Copy the old block
        memcpy(new_triangles, _triangles, min(triangle_size, _triangle_count * sizeof(ivec3)));
        for (size_t i = 0; i < _attributes.count(); ++i)
            memcpy(new_buffers[i].data, buffers[i].data, min(new_buffers[i].size, buffers[i].size));

        // Delete the old block
        delete[] block;
        //free(block);
    }

    // Update pointers & sizes
    block = new_block;
    buffers = new_buffers;
    _triangles = new_triangles;
    _vertex_count = vertex_count;
    _triangle_count = triangle_count;
    _finalized = false;
}

void Mesh::append(const Mesh& other) {
    #ifdef FRAME_ASSERTS
    assert(_attributes == other._attributes);
    #endif

    // Resize to make room for the new guy
    size_t offsets[] = { _vertex_count, _triangle_count };
    resize(offsets[0] + other._vertex_count,
           offsets[1] + other._triangle_count);

    // Copy vertex data
    for (size_t i = 0; i < _attributes.count(); ++i) {
        memcpy(buffers[i].data + offsets[0] * _attributes[i].size, other.buffers[i].data, other.buffers[i].size);
    }

    // Copy triangle data
    memcpy(_triangles + offsets[1], other._triangles, other._triangle_count * sizeof(ivec3));

    // Fix indexes
    for (size_t i = 0; i < other._triangle_count; ++i)
        _triangles[offsets[1] + i] += ivec3(offsets[0]);

    unfinalize();
}

void Mesh::create_buffers() {

    // Create & bind a vertex array object
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Create vertex buffers & set up array attributes
    for (size_t i = 0; i < _attributes.count(); ++i) {
        glGenBuffers(1, &buffers[i].vbo);
        glBindBuffer(GL_ARRAY_BUFFER, buffers[i].vbo);
        glBufferData(GL_ARRAY_BUFFER, buffers[i].size, buffers[i].data, _attributes[i].dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
        glEnableVertexAttribArray(i);
        glVertexAttribPointer(i, _attributes[i].size / sizeof(float), GL_FLOAT, GL_FALSE, 0, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    // Create a buffer for triangle indices
    glGenBuffers(1, &vbo_triangles);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_triangles);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(ivec3) * _triangle_count, _triangles, _dynamic_triangles ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // Unbind the vao
    glBindVertexArray(0);
    gl_check();
}

void Mesh::update_vertex_buffers() { update_vertex_buffers(0, _vertex_count); }

void Mesh::update_vertex_buffers(size_t i) { update_vertex_buffers(i, i+1); }

void Mesh::update_vertex_buffers(size_t i0, size_t i1) {
    glBindVertexArray(vao);
    for (size_t i = 0; i < _attributes.count(); ++i) {
        size_t size = (i1 - i0) * _attributes[i].size;
        glBindBuffer(GL_ARRAY_BUFFER, buffers[i].vbo);
        glBufferSubData(GL_ARRAY_BUFFER, i0 * _attributes[i].size, size, buffers[i].data);
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    gl_check();
}

void Mesh::update_vertex_buffer(size_t i, size_t i0, size_t i1) {
    size_t size = (i1 - i0) * _attributes[i].size;
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, buffers[i].vbo);
    glBufferSubData(GL_ARRAY_BUFFER, i0 * _attributes[i].size, size, buffers[i].data);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    gl_check();
}

void Mesh::update_index_buffer() { update_index_buffer(0, _triangle_count); }

void Mesh::update_index_buffer(size_t i) { update_index_buffer(i, i+1); }

void Mesh::update_index_buffer(size_t i0, size_t i1) {
    size_t size = (i1 - i0) * sizeof(ivec3);
    glBindVertexArray(vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_triangles);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, i0 * sizeof(ivec3), size, _triangles);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    gl_check();
}

void Mesh::destroy_buffers() {

    // Destroy index buffer
    glDeleteBuffers(1, &vbo_triangles);

    // Destroy all vertex buffers
    for (size_t i = 0; i < _attributes.count(); ++i) {
        glDeleteBuffers(1, &buffers[i].vbo);
    }

    // Destroy vertex array object
    glDeleteVertexArrays(1, &vao);
}
