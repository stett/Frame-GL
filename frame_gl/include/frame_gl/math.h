#pragma once
#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/norm.hpp"

namespace frame
{
    // Constants
    const float pi = 3.14159265359f;

    // GLM
    typedef glm::vec2 vec2;
    typedef glm::vec3 vec3;
    typedef glm::vec4 vec4;
    typedef glm::ivec2 ivec2;
    typedef glm::ivec3 ivec3;
    typedef glm::ivec4 ivec4;
    typedef glm::mat2 mat2;
    typedef glm::mat3 mat3;
    typedef glm::mat4 mat4;
    using glm::max;
    using glm::min;
    using glm::cross;
    using glm::dot;
    using glm::sqrt;
    using glm::length;
    using glm::normalize;
    using glm::cos;
    using glm::sin;

    // GTC
    typedef glm::quat quat;

    // GTX
    using glm::length2;


    // Functions

    // From: http://stackoverflow.com/a/11741520/1432965
    vec3 orthogonal(const vec3& v);
    quat rotation_between(const vec3& u, const vec3& v);

    // From: http://stett.github.io/projecting-screen-coordinates-onto-a-3d-plane
    void homogeneous_to_world(vec3 &world, const vec3 &homogeneous, const mat4 &projection, const mat4 &view);
    bool project_onto_plane(vec3 &point, const vec2 &view_point, const vec3 &plane_point, const vec3 &plane_normal, const mat4 &projection, const mat4 &view);
}