#pragma once
#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/norm.hpp"
#include "glm/gtx/hash.hpp"
#include "frame_gl/quat.h"
#include "frame_gl/raw.h"

namespace frame
{
    // Constants
    const float pi = 3.14159265359f;

    // GLM
    using glm::vec2;
    using glm::vec3;
    using glm::vec4;
    using glm::ivec2;
    using glm::ivec3;
    using glm::ivec4;
    using glm::mat2;
    using glm::mat3;
    using glm::mat4;
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
    //typedef glm::quat quat;

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