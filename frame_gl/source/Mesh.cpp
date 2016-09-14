#define GLEW_STATIC
#define _CRT_SECURE_NO_WARNINGS
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

Mesh::Mesh() : vao(0), vbo_positions(0), vbo_normals(0), vbo_uvs(0), vbo_colors(0), vbo_indices(0) {
    if (glfwGetCurrentContext() == 0)
        Log::error("Can't create a mesh outside of an OpenGL context!");

    build_gl_objects();
}

Mesh::~Mesh() { release_gl_objects(); }

void Mesh::render() {
    glBindVertexArray(vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_indices);

    if (triangles.size() > 0)
        glDrawElements(GL_TRIANGLES, 3 * triangles.size(), GL_UNSIGNED_INT, 0);
    else if (lines.size() > 0)
        glDrawElements(GL_LINES, 2 * lines.size(), GL_UNSIGNED_INT, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Mesh::clear() {
    positions.clear();
    normals.clear();
    uvs.clear();
    colors.clear();
    triangles.clear();
    release_gl_objects();
}

void Mesh::add_position(const vec3& position) {
    positions.push_back(position);
}

void Mesh::add_uv(const vec2& uv) {
    uvs.push_back(uv);
}

void Mesh::add_normal(const vec3& normal) {
    normals.push_back(normal);
}

void Mesh::add_vertex(const vec3& position, const vec3& normal, const vec2& uv, const vec4& color) {
    add_position(position);
    add_normal(normal);
    add_uv(uv);
    colors.push_back(color);
}

void Mesh::add_triangle(const ivec3& indices) {
    triangles.push_back(indices);
    build_gl_objects();
}

void Mesh::add_line(const ivec2& indices) {
    lines.push_back(indices);
    build_gl_objects();
}

void Mesh::load_obj_str(const std::string& obj, bool _normalize, bool _center) {
    clear();

    std::stringstream ifs(obj);
    //std::istream(obj.begin());
    //std::ifstream ifs(obj);
    //std::ifstream ifs(filename);
    /*if (!ifs.is_open()) {
        Log::error("Failed to open file: " + filename);
        return mesh;
    }*/

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

    build_gl_objects();
}

void Mesh::build_gl_objects() {

    // Release the old GL objects if there are any
    release_gl_objects();

    // Make a new VAO & bind it
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Make the vertex position buffer, fill it with our vertex data,
    // and enable the attribute for the VAO
    if (positions.size() > 0) {
        glGenBuffers(1, &vbo_positions);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_positions);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * positions.size(), positions.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    if (normals.size() > 0) {
        glGenBuffers(1, &vbo_normals);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_normals);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * normals.size(), normals.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    if (uvs.size() > 0) {
        glGenBuffers(1, &vbo_uvs);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_uvs);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vec2) * uvs.size(), uvs.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    if (colors.size() > 0) {
        glGenBuffers(1, &vbo_colors);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_colors);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vec4) * colors.size(), colors.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 0, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    if (triangles.size() > 0) {
        glGenBuffers(1, &vbo_indices);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_indices);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(ivec3) * triangles.size(), triangles.data(), GL_STATIC_DRAW);
        //
        // TODO: Test this unbind: is it necessary? (!!!)
        //
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    if (lines.size() > 0 && triangles.size() == 0) {
        glGenBuffers(1, &vbo_indices);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_indices);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(ivec2) * lines.size(), lines.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    // Unbind the VAO
    glBindVertexArray(0);
    gl_check();
}

void Mesh::release_gl_objects() {

    // First release all the buffer objects, and then the array object
    if (vbo_positions)  glDeleteBuffers(1, &vbo_positions);
    if (vbo_normals)    glDeleteBuffers(1, &vbo_normals);
    if (vbo_uvs)        glDeleteBuffers(1, &vbo_uvs);
    if (vbo_colors)     glDeleteBuffers(1, &vbo_colors);
    if (vbo_indices)    glDeleteBuffers(1, &vbo_indices);
    if (vao)            glDeleteVertexArrays(1, &vao);

    // Reset handles so they don't get deleted twice
    vbo_positions =
    vbo_normals =
    vbo_uvs =
    vbo_colors =
    vbo_indices =
    vao = 0;

    // Make sure nothing went wrong
    gl_check();
}


//
// Mesh Factories
//

Resource<Mesh> Mesh::Factory::load(const std::string& filename, bool normalize, bool center) {
    Resource<Mesh> mesh;
    std::ifstream ifs(filename);
    if (!ifs.is_open()) {
        Log::error("Failed to open file: " + filename);
        return mesh;
    }
    std::string obj = std::string(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>());
    ifs.close();
    mesh->load_obj_str(obj, normalize, center);
    return mesh;
}

Resource<Mesh> Mesh::Factory::combine(const std::vector< Resource<Mesh> >& meshes) {
    Resource<Mesh> combined;
    for (auto mesh : meshes) {
        int offset = combined->positions.size();
        combined->positions.insert(combined->positions.end(), mesh->positions.begin(), mesh->positions.end());
        combined->normals.insert(combined->normals.end(), mesh->normals.begin(), mesh->normals.end());
        combined->uvs.insert(combined->uvs.end(), mesh->uvs.begin(), mesh->uvs.end());
        combined->colors.insert(combined->colors.end(), mesh->colors.begin(), mesh->colors.end());
        for (auto triangle : mesh->triangles)
            combined->add_triangle(triangle + ivec3(offset));
    }
    return combined;
}

Resource<Mesh> Mesh::Factory::rectangle(const vec2& size, const vec3& center) {
    Resource<Mesh> mesh;
    vec3 normal(0.0f, 0.0f, 1.0f);
    mesh->add_vertex(center + vec3(-size.x, -size.y, 0.0f) * 0.5f, normal, vec2(0.0f, 0.0f));
    mesh->add_vertex(center + vec3(-size.x,  size.y, 0.0f) * 0.5f, normal, vec2(0.0f, 1.0f));
    mesh->add_vertex(center + vec3( size.x,  size.y, 0.0f) * 0.5f, normal, vec2(1.0f, 1.0f));
    mesh->add_vertex(center + vec3( size.x, -size.y, 0.0f) * 0.5f, normal, vec2(1.0f, 0.0f));
    mesh->add_triangle(0, 1, 2);
    mesh->add_triangle(0, 2, 3);
    return mesh;
}

Resource<Mesh> Mesh::Factory::circle(float radius, const vec3& center, float verts_per_length) {
    Resource<Mesh> mesh;

    vec3 normal(0.0f, 0.0f, 1.0f);
    float circumference = 2.0f * pi * radius;
    std::size_t count = (std::size_t)(verts_per_length * circumference);

    // Build the vertices
    for (std::size_t index = 0; index < count; ++index) {
        float angle = (float)index / (float)count;
        vec3 position = vec3(glm::cos(angle), glm::sin(angle), 0.0f) * radius;
        vec2 uv = vec3(glm::cos(angle), -glm::sin(angle), 0.0f) * 0.5f + vec3(0.5f, 0.5f, 0.0f);
        mesh->add_vertex(position, normal, uv);
    }

    // Build the indices
    for (std::size_t index = 2; index < count; ++index)
        mesh->add_triangle(0, index - 1, index);

    return mesh;
}

Resource<Mesh> Mesh::Factory::arrow(const vec3& base, const vec3& tip, const vec4& color, float size) {
    //
    // TODO: Add support for line and point style meshes
    //       so that it's not necessary to hack this!!
    //
    Resource<Mesh> mesh;
    vec3 norm = normalize(tip - base);
    vec3 perp1 = orthogonal(norm);
    vec3 perp2 = cross(norm, perp1);//orthogonal(perp1);
    vec3 head1 = tip + size * (perp1 - norm);
    vec3 head2 = tip + size * (-perp1 - norm);
    vec3 head3 = tip + size * (perp2 - norm);
    vec3 head4 = tip + size * (-perp2 - norm);
    vec3 subtip = tip + norm * dot(norm, size * (perp1 - norm));
    mesh->add_vertex(base, vec3(0.0f), vec2(0.0f), color);
    mesh->add_vertex(tip, vec3(0.0f), vec2(0.0f), color);
    mesh->add_vertex(subtip, vec3(0.0f), vec2(0.0f), color);
    mesh->add_vertex(head1, vec3(0.0f), vec2(0.0f), color);
    mesh->add_vertex(head2, vec3(0.0f), vec2(0.0f), color);
    mesh->add_vertex(head3, vec3(0.0f), vec2(0.0f), color);
    mesh->add_vertex(head4, vec3(0.0f), vec2(0.0f), color);
    mesh->add_triangle(0, 2, 2);
    mesh->add_triangle(1, 3, 4);
    mesh->add_triangle(1, 5, 6);

    return mesh;
}

Resource<Mesh> Mesh::Factory::cube(float edge, const vec4& color, bool smooth) {
    Resource<Mesh> mesh;
    float half = edge * 0.5f;

    if (smooth) {

        float normag = sqrt(3 * half * half);

        mesh->add_vertex(vec3(-half, -half, -half), vec3(-normag, -normag, -normag), vec3(0.0f), color);
        mesh->add_vertex(vec3(half, -half, -half), vec3(normag, -normag, -normag), vec3(0.0f), color);
        mesh->add_vertex(vec3(-half, half, -half), vec3(-normag, normag, -normag), vec3(0.0f), color);
        mesh->add_vertex(vec3(half, half, -half), vec3(normag, normag, -normag), vec3(0.0f), color);

        mesh->add_vertex(vec3(-half, -half, half), vec3(-normag, -normag, normag), vec3(0.0f), color);
        mesh->add_vertex(vec3(half, -half, half), vec3(normag, -normag, normag), vec3(0.0f), color);
        mesh->add_vertex(vec3(-half, half, half), vec3(-normag, normag, normag), vec3(0.0f), color);
        mesh->add_vertex(vec3(half, half, half), vec3(normag, normag, normag), vec3(0.0f), color);

        // back
        mesh->add_triangle(0, 1, 2);
        mesh->add_triangle(1, 2, 3);

        // bottom
        mesh->add_triangle(0, 1, 4);
        mesh->add_triangle(1, 4, 5);

        // front
        mesh->add_triangle(4, 5, 6);
        mesh->add_triangle(5, 6, 7);

        // top
        mesh->add_triangle(7, 3, 6);
        mesh->add_triangle(3, 6, 2);

        // left
        mesh->add_triangle(0, 2, 4);
        mesh->add_triangle(2, 4, 6);

        // right
        mesh->add_triangle(1, 3, 5);
        mesh->add_triangle(3, 5, 7);

    } else {

        // 0
        mesh->add_vertex(vec3(-half, -half, -half), vec3(-1.0f, 0.0f, 0.0f), vec3(0.0f), color);
        mesh->add_vertex(vec3(-half, -half, -half), vec3(0.0f, -1.0f, 0.0f), vec3(0.0f), color);
        mesh->add_vertex(vec3(-half, -half, -half), vec3(0.0f, 0.0f, -1.0f), vec3(0.0f), color);

        // 1
        mesh->add_vertex(vec3(half, -half, -half), vec3(1.0f, 0.0f, 0.0f), vec3(0.0f), color);
        mesh->add_vertex(vec3(half, -half, -half), vec3(0.0f, -1.0f, 0.0f), vec3(0.0f), color);
        mesh->add_vertex(vec3(half, -half, -half), vec3(0.0f, 0.0f, -1.0f), vec3(0.0f), color);

        // 2
        mesh->add_vertex(vec3(-half, half, -half), vec3(-1.0f, 0.0f, 0.0f), vec3(0.0f), color);
        mesh->add_vertex(vec3(-half, half, -half), vec3(0.0f, 1.0f, 0.0f), vec3(0.0f), color);
        mesh->add_vertex(vec3(-half, half, -half), vec3(0.0f, 0.0f, -1.0f), vec3(0.0f), color);

        // 3
        mesh->add_vertex(vec3(half, half, -half), vec3(1.0f, 0.0f, 0.0f), vec3(0.0f), color);
        mesh->add_vertex(vec3(half, half, -half), vec3(0.0f, 1.0f, 0.0f), vec3(0.0f), color);
        mesh->add_vertex(vec3(half, half, -half), vec3(0.0f, 0.0f, -1.0f), vec3(0.0f), color);

        // 4
        mesh->add_vertex(vec3(-half, -half, half), vec3(-1.0f, 0.0f, 0.0f), vec3(0.0f), color);
        mesh->add_vertex(vec3(-half, -half, half), vec3(0.0f, -1.0f, 0.0f), vec3(0.0f), color);
        mesh->add_vertex(vec3(-half, -half, half), vec3(0.0f, 0.0f, 1.0f), vec3(0.0f), color);

        // 5
        mesh->add_vertex(vec3(half, -half, half), vec3(1.0f, 0.0f, 0.0f), vec3(0.0f), color);
        mesh->add_vertex(vec3(half, -half, half), vec3(0.0f, -1.0f, 0.0f), vec3(0.0f), color);
        mesh->add_vertex(vec3(half, -half, half), vec3(0.0f, 0.0f, 1.0f), vec3(0.0f), color);

        // 6
        mesh->add_vertex(vec3(-half, half, half), vec3(-1.0f, 0.0f, 0.0f), vec3(0.0f), color);
        mesh->add_vertex(vec3(-half, half, half), vec3(0.0f, 1.0f, 0.0f), vec3(0.0f), color);
        mesh->add_vertex(vec3(-half, half, half), vec3(0.0f, 0.0f, 1.0f), vec3(0.0f), color);

        // 7
        mesh->add_vertex(vec3(half, half, half), vec3(1.0f, 0.0f, 0.0f), vec3(0.0f), color);
        mesh->add_vertex(vec3(half, half, half), vec3(0.0f, 1.0f, 0.0f), vec3(0.0f), color);
        mesh->add_vertex(vec3(half, half, half), vec3(0.0f, 0.0f, 1.0f), vec3(0.0f), color);

        // back
        mesh->add_triangle((0*3)+2, (1*3)+2, (2*3)+2);
        mesh->add_triangle((1*3)+2, (2*3)+2, (3*3)+2);

        // bottom
        mesh->add_triangle((0*3)+1, (1*3)+1, (4*3)+1);
        mesh->add_triangle((1*3)+1, (4*3)+1, (5*3)+1);

        // front
        mesh->add_triangle((4*3)+2, (5*3)+2, (6*3)+2);
        mesh->add_triangle((5*3)+2, (6*3)+2, (7*3)+2);

        // top
        mesh->add_triangle((7*3)+1, (3*3)+1, (6*3)+1);
        mesh->add_triangle((3*3)+1, (6*3)+1, (2*3)+1);

        // left
        mesh->add_triangle(0*3, 2*3, 4*3);
        mesh->add_triangle(2*3, 4*3, 6*3);

        // right
        mesh->add_triangle(1*3, 3*3, 5*3);
        mesh->add_triangle(3*3, 5*3, 7*3);
    }

    return mesh;
}

Resource<Mesh> Mesh::Factory::quad(const vec2& size, const vec3& normal, const vec4& color) {
    Resource<Mesh> mesh;
    vec2 half = size * 0.5f;

    //
    // TODO: Transform this to face in the normal direction
    //

    mesh->add_vertex(vec3(-half.x, half.y, 0.0f), vec3(0.0f), vec3(0.0f), color);
    mesh->add_vertex(vec3(half.x, half.y, 0.0f), vec3(0.0f), vec3(0.0f), color);
    mesh->add_vertex(vec3(-half.x, -half.y, 0.0f), vec3(0.0f), vec3(0.0f), color);
    mesh->add_vertex(vec3(half.x, -half.y, 0.0f), vec3(0.0f), vec3(0.0f), color);
    mesh->add_triangle(2, 1, 0);
    mesh->add_triangle(1, 2, 3);
    return mesh;
}

Resource<Mesh> Mesh::Factory::sphere(float radius, int recursion, const vec4& color) {
    Resource<Mesh> mesh;
    std::unordered_map<ivec2, int> midpoint_cache;
    std::vector<ivec3>* faces = new std::vector<ivec3>();
    std::vector<vec3> vertices;
    int index = 0;

    // Create 12 vertices of icosahedron
    float t = (1.0f + sqrt(5.0f)) * 0.5f;
          
    vertices.push_back(vec3(-1.0f,  t, 0.0f));
    vertices.push_back(vec3( 1.0f,  t, 0.0f));
    vertices.push_back(vec3(-1.0f, -t, 0.0f));
    vertices.push_back(vec3( 1.0f, -t, 0.0f));

    vertices.push_back(vec3(0.0f, -1.0f,  t));
    vertices.push_back(vec3(0.0f,  1.0f,  t));
    vertices.push_back(vec3(0.0f, -1.0f, -t));
    vertices.push_back(vec3(0.0f,  1.0f, -t));

    vertices.push_back(vec3( t, 0.0f, -1.0f));
    vertices.push_back(vec3( t, 0.0f,  1.0f));
    vertices.push_back(vec3(-t, 0.0f, -1.0f));
    vertices.push_back(vec3(-t, 0.0f,  1.0f));

    // 5 faces around point 0
    faces->push_back(ivec3(0, 11, 5));
    faces->push_back(ivec3(0, 5, 1));
    faces->push_back(ivec3(0, 1, 7));
    faces->push_back(ivec3(0, 7, 10));
    faces->push_back(ivec3(0, 10, 11));

    // 5 adjacent faces 
    faces->push_back(ivec3(1, 5, 9));
    faces->push_back(ivec3(5, 11, 4));
    faces->push_back(ivec3(11, 10, 2));
    faces->push_back(ivec3(10, 7, 6));
    faces->push_back(ivec3(7, 1, 8));

    // 5 faces around point 3
    faces->push_back(ivec3(3, 9, 4));
    faces->push_back(ivec3(3, 4, 2));
    faces->push_back(ivec3(3, 2, 6));
    faces->push_back(ivec3(3, 6, 8));
    faces->push_back(ivec3(3, 8, 9));

    // 5 adjacent faces 
    faces->push_back(ivec3(4, 9, 5));
    faces->push_back(ivec3(2, 4, 11));
    faces->push_back(ivec3(6, 2, 10));
    faces->push_back(ivec3(8, 6, 7));
    faces->push_back(ivec3(9, 8, 1));

    // refine triangles
    for (int i = 0; i < recursion; i++)
    {
        std::vector<ivec3>* faces2 = new std::vector<ivec3>();
        for (const ivec3& tri : *faces)
        {
            // replace triangle by 4 triangles
            int a = get_midpoint(midpoint_cache, vertices, ivec2(tri[0], tri[1]));
            int b = get_midpoint(midpoint_cache, vertices, ivec2(tri[1], tri[2]));
            int c = get_midpoint(midpoint_cache, vertices, ivec2(tri[2], tri[0]));

            faces2->push_back(ivec3(tri[0], a, c));
            faces2->push_back(ivec3(tri[1], b, a));
            faces2->push_back(ivec3(tri[2], c, b));
            faces2->push_back(ivec3(a, b, c));
        }
        delete faces;
        faces = faces2;
    }

    // Add triangles to mesh
    for (const vec3& position : vertices) {
        mesh->add_position(position * 0.5f * radius);
        mesh->add_normal(normalize(position));
    }

    for (const ivec3& face : *faces) {
        mesh->add_triangle(face);
    }

    delete faces;

    return mesh;
}

int Mesh::Factory::get_midpoint(std::unordered_map<ivec2, int>& cache, std::vector<vec3>& vertices, ivec2 indices) {

    if (indices.x < indices.y)
        std::swap(indices.x, indices.y);

    auto it = cache.find(indices);
    if (it != cache.end())
        return it->second;

    // Not in cache, calculate it
    int index = vertices.size();
    cache[indices] = index;
    vec3 vert = (vertices[indices.x] + vertices[indices.y]) * 0.5f;
    vertices.push_back(normalize(vert) * 2.0f);

    return index;
}
