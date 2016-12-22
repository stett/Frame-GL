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
#include "frame_gl/data/MeshFactory.h"
#include "frame_gl/math.h"
#include "frame_gl/error.h"
using namespace frame;

namespace
{
    int get_midpoint(std::unordered_map<ivec2, int>& cache, std::vector<vec3>& vertices, ivec2 indices) {
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
}

Resource<Mesh> MeshFactory::load_obj_file(const std::string& filename, bool normalize, bool center) {
    Resource<Mesh> mesh;
    std::ifstream ifs(filename);
    if (!ifs.is_open()) {
        Log::error("Failed to open file: " + filename);
        return mesh;
    }
    std::string obj = std::string(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>());
    ifs.close();
    return load_obj_string(obj, normalize, center);
}

Resource<Mesh> MeshFactory::load_obj_string(const std::string& obj, bool normalize, bool center) {
    return Resource<Mesh>();

    /*
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
    */
}

Resource<Mesh> MeshFactory::combine(const std::vector< Resource<Mesh> >& meshes, bool dynamic_triangles) {
    Resource<Mesh> combined(meshes[0]->attributes());
    for (auto mesh : meshes)
        combined->append(*mesh);
    return combined;
}

Resource<Mesh> MeshFactory::rectangle(const vec2& size, const vec3& center) {
    Resource<Mesh> mesh(4, 2);
    vec3 normal(0.0f, 0.0f, 1.0f);
    mesh->set_vertices(
        {   center + vec3(-size.x, -size.y, 0.0f) * 0.5f,
            center + vec3(-size.x,  size.y, 0.0f) * 0.5f,
            center + vec3( size.x,  size.y, 0.0f) * 0.5f,
            center + vec3( size.x, -size.y, 0.0f) * 0.5f },
        { normal, normal, normal, normal },
        { vec2(0.0f, 0.0f), vec2(0.0f, 1.0f), vec2(1.0f, 1.0f), vec2(1.0f, 0.0f) });
    mesh->set_triangles({ ivec3(0, 1, 2), ivec3(0, 2, 3) });
    return mesh;
}

Resource<Mesh> MeshFactory::circle(float radius, const vec3& center, float verts_per_length) {

    vec3 normal(0.0f, 0.0f, 1.0f);
    float circumference = 2.0f * pi * radius;
    std::size_t count = (std::size_t)(verts_per_length * circumference);

    Resource<Mesh> mesh(count, count - 2);

    // Build the vertices
    for (std::size_t index = 0; index < count; ++index) {
        float angle = (float)index / (float)count;
        vec3 position = vec3(glm::cos(angle), glm::sin(angle), 0.0f) * radius;
        vec2 uv = vec3(glm::cos(angle), -glm::sin(angle), 0.0f) * 0.5f + vec3(0.5f, 0.5f, 0.0f);
        mesh->set_vertex(index, position, normal, uv);
    }

    // Build the indices
    for (std::size_t index = 2; index < count; ++index)
        mesh->set_triangle(index - 2, ivec3(0, index - 1, index));

    return mesh;
}

Resource<Mesh> MeshFactory::arrow(const vec3& base, const vec3& tip, const vec4& color, float size) {

    //
    // TODO: Add support for line and point style meshes
    //       so that it's not necessary to hack this!!
    //
    Resource<Mesh> mesh(7, 3);
    vec3 norm = normalize(tip - base);
    vec3 perp1 = orthogonal(norm);
    vec3 perp2 = cross(norm, perp1);
    vec3 head1 = tip + size * (perp1 - norm);
    vec3 head2 = tip + size * (-perp1 - norm);
    vec3 head3 = tip + size * (perp2 - norm);
    vec3 head4 = tip + size * (-perp2 - norm);
    vec3 subtip = tip + norm * dot(norm, size * (perp1 - norm));

    mesh->set_vertices(
        { base, tip, subtip, head1, head2, head3, head4 },
        { vec3(0.0f), vec3(0.0f), vec3(0.0f), vec3(0.0f), vec3(0.0f), vec3(0.0f), vec3(0.0f) },
        { vec2(0.0f), vec2(0.0f), vec2(0.0f), vec2(0.0f), vec2(0.0f), vec2(0.0f), vec2(0.0f) },
        { color, color, color, color, color, color, color });
    mesh->set_triangles({ ivec3(0, 2, 2), ivec3(1, 3, 4), ivec3(1, 5, 6) });

    return mesh;
}

Resource<Mesh> MeshFactory::cube(float edge, const vec4& color, bool smooth) {
    float half = edge * 0.5f;

    if (smooth) {

        Resource<Mesh> mesh(8, 12);

        float normag = sqrt(3 * half * half);

        mesh->set_vertices({
            vec3(-half, -half, -half), vec3(half, -half, -half),
            vec3(-half, half, -half), vec3(half, half, -half),
            vec3(-half, -half, half), vec3(half, -half, half),
            vec3(-half, half, half), vec3(half, half, half)
        }, {
            vec3(-normag, -normag, -normag), vec3(normag, -normag, -normag),
            vec3(-normag, normag, -normag), vec3(normag, normag, -normag),
            vec3(-normag, -normag, normag), vec3(normag, -normag, normag),
            vec3(-normag, normag, normag), vec3(normag, normag, normag)
        }, {
            vec2(0.0f), vec2(0.0f),
            vec2(0.0f), vec2(0.0f),
            vec2(0.0f), vec2(0.0f),
            vec2(0.0f), vec2(0.0f),
        }, {
            color, color, color, color,
            color, color, color, color
        });

        mesh->set_triangles({
            ivec3(0, 1, 2), ivec3(1, 2, 3), // back
            ivec3(0, 1, 4), ivec3(1, 4, 5), // bottom
            ivec3(4, 5, 6), ivec3(5, 6, 7), // front
            ivec3(7, 3, 6), ivec3(3, 6, 2), // top
            ivec3(0, 2, 4), ivec3(2, 4, 6), // left
            ivec3(1, 3, 5), ivec3(3, 5, 7)  // right
        });

        return mesh;

    } else {

        Resource<Mesh> mesh(24, 12);

        mesh->set_vertices({
            vec3(-half, -half, -half), vec3(-half, -half, -half), vec3(-half, -half, -half),
            vec3(half, -half, -half), vec3(half, -half, -half), vec3(half, -half, -half),
            vec3(-half, half, -half), vec3(-half, half, -half), vec3(-half, half, -half),
            vec3(half, half, -half), vec3(half, half, -half), vec3(half, half, -half),
            vec3(-half, -half, half), vec3(-half, -half, half), vec3(-half, -half, half),
            vec3(half, -half, half), vec3(half, -half, half), vec3(half, -half, half),
            vec3(-half, half, half), vec3(-half, half, half), vec3(-half, half, half),
            vec3(half, half, half), vec3(half, half, half), vec3(half, half, half)
        }, {
            vec3(-1.0f, 0.0f, 0.0f), vec3(0.0f, -1.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f),
            vec3(1.0f, 0.0f, 0.0f), vec3(0.0f, -1.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f),
            vec3(-1.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f),
            vec3(1.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f),
            vec3(-1.0f, 0.0f, 0.0f), vec3(0.0f, -1.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f),
            vec3(1.0f, 0.0f, 0.0f), vec3(0.0f, -1.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f),
            vec3(-1.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f),
            vec3(1.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f)
        }, {
            vec2(0.0f), vec2(0.0f), vec2(0.0f), vec2(0.0f), vec2(0.0f), vec2(0.0f),
            vec2(0.0f), vec2(0.0f), vec2(0.0f), vec2(0.0f), vec2(0.0f), vec2(0.0f),
            vec2(0.0f), vec2(0.0f), vec2(0.0f), vec2(0.0f), vec2(0.0f), vec2(0.0f),
            vec2(0.0f), vec2(0.0f), vec2(0.0f), vec2(0.0f), vec2(0.0f), vec2(0.0f),
        }, {
            color, color, color, color, color, color,
            color, color, color, color, color, color,
            color, color, color, color, color, color,
            color, color, color, color, color, color
        });

        mesh->set_triangles({

            // back
            ivec3((0*3)+2, (2*3)+2, (1*3)+2),
            ivec3((1*3)+2, (2*3)+2, (3*3)+2),

            // bottom
            ivec3((0*3)+1, (1*3)+1, (4*3)+1),
            ivec3((1*3)+1, (5*3)+1, (4*3)+1),

            // front
            ivec3((4*3)+2, (5*3)+2, (6*3)+2),
            ivec3((5*3)+2, (7*3)+2, (6*3)+2),

            // top
            ivec3((7*3)+1, (3*3)+1, (6*3)+1),
            ivec3((3*3)+1, (2*3)+1, (6*3)+1),

            // left
            ivec3(0*3, 4*3, 2*3),
            ivec3(2*3, 4*3, 6*3),

            // right
            ivec3(1*3, 3*3, 5*3),
            ivec3(3*3, 7*3, 5*3)
        });

        return mesh;
    }
}

Resource<Mesh> MeshFactory::quad(const vec2& size, const vec3& normal, const vec4& color) {
    Resource<Mesh> mesh(4, 2);
    vec2 half = size * 0.5f;

    //
    // TODO: Transform this to face in the normal direction
    //

    mesh->set_vertices({
        vec3(-half.x, half.y, 0.0f), vec3(half.x, half.y, 0.0f),
        vec3(-half.x, -half.y, 0.0f), vec3(half.x, -half.y, 0.0f) },
    { vec3(0.0f), vec3(0.0f), vec3(0.0f), vec3(0.0f) },
    { vec3(0.0f), vec3(0.0f), vec3(0.0f), vec3(0.0f) },
    { color, color, color, color });

    mesh->set_triangles({ ivec3(2, 1, 0), ivec3(1, 2, 3) });

    return mesh;
}

Resource<Mesh> MeshFactory::sphere(float radius, int recursion, const vec4& color) {
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

    // build the mesh
    Resource<Mesh> mesh(vertices.size(), faces->size());
    for (size_t i = 0; i < vertices.size(); ++i)
        mesh->set_vertex(i, vertices[i] * 0.5f * radius, normalize(vertices[i]));
    for (size_t i = 0; i < faces->size(); ++i)
        mesh->set_triangle(i, faces->at(i));

    delete faces;
    return mesh;
}
