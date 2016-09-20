#pragma once
#include "glm/gtc/matrix_transform.hpp"
#include "frame/Component.h"
#include "frame_gl/math.h"
#include "frame_gl/components/Transform.h"
#include "frame_gl/components/RenderTarget.h"

namespace frame
{
    FRAME_COMPONENT(Camera, Transform, RenderTarget) {
    public:
        Camera(float vertical_fov=120.0f, float aspect_ratio=1.0f, float clip_near=0.1f, float clip_far=2000.0f) :
            _projection_matrix(glm::perspective(vertical_fov, aspect_ratio, clip_near, clip_far)) {}
        Camera(const vec2& ortho_size, float clip_near=0.0f/*-1000.0f*/, float clip_far=2000.0f) :
            _projection_matrix(glm::ortho(-ortho_size.x * 0.5f, ortho_size.x * 0.5f, -ortho_size.y * 0.5f, ortho_size.y * 0.5f, clip_near, clip_far)) {}

    public:
        void bind_target(bool clear=false) {
            auto target = get<RenderTarget>();
            get<RenderTarget>()->bind_target(clear);
        }

        void unbind_target() {
            get<RenderTarget>()->unbind_target();
        }

        /// \brief Project a point from the viewing plane onto a plane in world space.
        /// \param world_point the output value.
        /// \param view_point the point on the homogeneous viewing plane. Must be between -1 and 1 on both axes.
        /// \param plane_point any point contained in projection plane.
        /// \param plane_normal surface normal for the projection plane.
        /// \return true if the ray interesects with the plane at all.
        bool get_world_point(vec3 &world_point, const vec2 &view_point, const vec3 &plane_point=vec3(0.0f), const vec3 &plane_normal=vec3(0.0f, 0.0f, 1.0f)) {
            return project_onto_plane(world_point, view_point, plane_point, plane_normal, projection_matrix(), view_matrix());
        }

        const mat4& view_matrix() { return get<Transform>()->world_inverse(); }
        const mat4& projection_matrix() { return _projection_matrix; }

    private:
        mat4 _projection_matrix;
    };
}