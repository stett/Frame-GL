#include "frame_gl/math.h"
using namespace frame;

vec3 frame::orthogonal(const vec3& v)
{
    float x = abs(v.x);
    float y = abs(v.y);
    float z = abs(v.z);
    vec3 other = x < y ? (x < z ? vec3(1.0f, 0.0f, 0.0f) : vec3(0.0f, 0.0f, 1.0f)) : (y < z ? vec3(0.0f, 1.0f, 0.0f) : vec3(0.0f, 0.0f, 1.0f));
    return cross(v, other);
}

quat frame::rotation_between(const vec3& u, const vec3& v) {
    float k_cos_theta = dot(u, v);
    float k = sqrt(length2(u) * length2(v));

    // 180 degree rotation around any orthogonal vector
    if (k_cos_theta / k == -1)
        return quat(0, normalize(orthogonal(u)));

    return quat(k_cos_theta + k, cross(u, v)).normalize();
}

void frame::homogeneous_to_world(vec3 &world, const vec3 &homogeneous, const mat4 &projection, const mat4 &view)
{
    mat4 transform = inverse(projection * view);
    vec4 _world = transform * vec4(homogeneous, 1.0f);
    world = vec3(_world) * (1.0f / _world.w);
}

bool frame::project_onto_plane(vec3 &point, const vec2 &view_point, const vec3 &plane_point, const vec3 &plane_normal, const mat4 &projection, const mat4 &view)
{
    vec3 ray_origin, ray_end;
    homogeneous_to_world(ray_origin, vec3(view_point, 0.0f), projection, view);
    homogeneous_to_world(ray_end, vec3(view_point, 1.0f), projection, view);

    vec3 ray_normal = normalize(ray_end - ray_origin);
    float t = dot(plane_point - ray_origin, plane_normal) / dot(ray_normal, plane_normal);
    point = ray_origin + t * ray_normal;

    return t >= 0.0f;
}
