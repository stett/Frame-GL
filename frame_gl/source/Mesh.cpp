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

Mesh::Mesh(VertexAttributeSet attributes, size_t vertex_count, size_t triangle_count, bool dynamic_triangles) :
    _attributes(attributes), _dynamic_triangles(dynamic_triangles), vao(0), block(0) {

    if (glfwGetCurrentContext() == 0)
        Log::error("Can't create a mesh outside of an OpenGL context!");

    // Set up empty buffers, & get space for them in gfx
    resize_block(vertex_count, triangle_count);
    create_buffers();
}

Mesh::~Mesh() {
    delete[] block;     // Delete our local buffers 
    destroy_buffers();  // Delete gfx buffers
}

void Mesh::bind() {
    glBindVertexArray(vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_triangles);
}

void Mesh::unbind() {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Mesh::draw() {
    glDrawElements(GL_TRIANGLES, 3 * _triangle_count, GL_UNSIGNED_INT, 0);
}

void Mesh::render() {
    bind();
    draw();
    unbind();
}

void Mesh::resize(size_t vertex_count, size_t triangle_count) {
    destroy_buffers();
    resize_block(vertex_count, triangle_count);
    create_buffers();
}

void Mesh::set_vertex_count(size_t vertex_count) {
    if (vertex_count != _vertex_count)
        resize(vertex_count, _triangle_count);
}

void Mesh::set_triangle_count(size_t triangle_count) {
    if (triangle_count != _triangle_count)
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

    // Set basic array locations
    VertexBuffer* new_buffers = (VertexBuffer*)(new_block);
    ivec3* new_triangles = (ivec3*)(new_block + buffers_size + vertex_size);

    // Set up buffers
    char* location = new_block + buffers_size;
    for (size_t i = 0; i < _attributes.count(); ++i) {
        new_buffers[i].data = location;
        new_buffers[i].size = _vertex_count * _attributes[i].size;
        location += _attributes[i].size;
    }

    if (block) {

        // Copy the old block
        memcpy(new_triangles, _triangles, _triangle_count * sizeof(ivec3));
        for (size_t i = 0; i < _attributes.count(); ++i)
            memcpy(new_buffers[i].data, buffers[i].data, _vertex_count * _attributes.size());

        // Delete the old block
        delete[] block;
    }

    // Update pointers & sizes
    block = new_block;
    buffers = new_buffers;
    _triangles = new_triangles;
    _vertex_count = vertex_count;
    _triangle_count = triangle_count;
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
}

void Mesh::update_buffers() { update_buffers(0, _vertex_count); }

void Mesh::update_buffers(size_t i) { update_buffers(i, i+1); }

void Mesh::update_buffers(size_t i0, size_t i1) {
    glBindVertexArray(vao);
    for (size_t i = 0; i < _attributes.count(); ++i) {
        size_t size = (i1 - i0) * _attributes[i].size;
        glBindBuffer(GL_ARRAY_BUFFER, buffers[i].vbo);
        glBufferSubData(GL_ARRAY_BUFFER, i0 * _attributes[i].size, size, buffers[i].data + size);
        glBufferData(GL_ARRAY_BUFFER, buffers[i].size, buffers[i].data, _attributes[i].dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Mesh::destroy_buffers() {

    // Destroy all vertex buffers
    for (size_t i = 0; i < _attributes.count(); ++i) {
        glDeleteBuffers(1, &buffers[i].vbo);
        buffers[i].vbo = 0;
    }

    // Destroy vertex array object
    glDeleteVertexArrays(1, &vao);
}

/*
void Mesh::load_obj_str(const std::string& obj, bool _normalize, bool _center) {
    clear();

    // Load OBJ files.
    vec2 input2f;
    vec3 input3f;
    ivec3 input3i;
    int input15i[15];
    char *pch;
    int vertexCount = 0;
    char buf[1024];
    vec3 minima;
    vec3 maxima;

    while (ifs.peek() != EOF) {

        // Parse every line until end of file
        ifs >> buf;
        if (buf[0] == '#') ifs.getline(buf, 1024); // comment block
        else if (strcmp(buf, "g") == 0) ifs.getline(buf, 1024); // group block
        else if (strcmp(buf, "s") == 0) ifs.getline(buf, 1024); 
        else if (strcmp(buf, "usemtl") == 0) ifs.getline(buf, 1024); // material block
        else if (strcmp(buf, "v") == 0) {

            // raw vertex posistion data parsing
            ifs >> input3f[0] >> input3f[1] >> input3f[2];
            positions.push_back(input3f);

            // record minima/maximua
            if (input3f.x < minima.x) minima.x = input3f.x;
            if (input3f.y < minima.y) minima.y = input3f.y;
            if (input3f.z < minima.z) minima.z = input3f.z;
            if (input3f.x > maxima.x) maxima.x = input3f.x;
            if (input3f.y > maxima.y) maxima.y = input3f.y;
            if (input3f.z > maxima.z) maxima.z = input3f.z;

        } else if (strcmp(buf, "vt") == 0) {

            // raw vertex uv data parsing
            ifs >> input2f[0] >> input2f[1];
            input2f[1] = 1 - input2f[1];
            uvs.push_back(input2f);

        } else if (strcmp(buf, "vn") == 0) {

            // raw vertex normal data parsing
            ifs >> input3f[0] >> input3f[1] >> input3f[2];
            normals.push_back(input3f);

        } else if (strcmp(buf, "f") == 0) {

            // raw face/index data parsing
            // Should consider both tris and quads
            ifs.getline(buf, 1024);
            char str[5][32];
            memset(str, 0, sizeof(char) * 5 * 32);
            pch = strtok(buf, " ");
            vertexCount = 0;

            // eliminate all extra spaces.
            while (pch) {
                strcpy(str[vertexCount++], pch);
                pch = strtok(NULL, " ");
            }

            int i = 0;
            int stride;
            for (int j = 0; j < vertexCount; ++j) {
                stride = 0;
                pch = strtok(str[j], "/");
                while (pch) {
                    ++stride;
                    input15i[i++] = std::stoi(pch);
                    pch = strtok(NULL, "/");
                }
            }

            input3i = ivec3(input15i[0], input15i[1*stride], input15i[2*stride]) - ivec3(1);
            triangles.push_back(input3i);
        }
    }

    //ifs.close();

    if (_normalize) {
        vec3 shift = _center ? -(minima + maxima) * 0.5f : vec3(0.0f);
        vec3 extrema = maxima - minima;
        float scale = 1.0f;
        for (int i = 0; i < 3; ++i)
            scale = min(scale, 2.0f / extrema[i]);
        for (vec3& pos : positions)
            pos = (pos + shift) * scale;
    }

    finalize();
}
*/