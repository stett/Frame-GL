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
        Camera(unsigned int layer=0, float vertical_fov=120.0f, float aspect_ratio=1.0f, float clip_near=0.1f, float clip_far=2000.0f)
        : _layer_mask(1 << layer), _projection_matrix(glm::perspective(vertical_fov, aspect_ratio, clip_near, clip_far)) {}
        Camera(unsigned int layer, const vec2& ortho_size, float clip_near=0.0f, float clip_far=2000.0f)
        : _layer_mask(1 << layer), _projection_matrix(glm::ortho(-ortho_size.x * 0.5f, ortho_size.x * 0.5f, -ortho_size.y * 0.5f, ortho_size.y * 0.5f, clip_near, clip_far)) {}
        Camera(unsigned int layer, const vec2& ortho_topleft, const vec2& ortho_bottomright, float clip_near=0.0f, float clip_far=2000.0f)
        : _layer_mask(1 << layer), _projection_matrix(glm::ortho(ortho_topleft.x, ortho_bottomright.x, ortho_bottomright.y, ortho_topleft.y, clip_near, clip_far)) {}

    public:
        void bind_target(bool clear=false) { get<RenderTarget>()->bind_target(clear); }

        void unbind_target() { get<RenderTarget>()->unbind_target(); }

        const RenderTarget* target() const { return get<RenderTarget>(); }

        /// \brief Project a point from the viewing plane onto a plane in world space.
        /// \param world_point the output value.
        /// \param view_point the point on the homogeneous viewing plane. Must be between -1 and 1 on both axes.
        /// \param plane_point any point contained in projection plane.
        /// \param plane_normal surface normal for the projection plane.
        /// \return true if the ray interesects with the plane at all.
        bool get_world_point(vec3 &world_point, const vec2 &view_point, const vec3 &plane_point=vec3(0.0f), const vec3 &plane_normal=vec3(0.0f, 0.0f, 1.0f)) {
            return project_onto_plane(world_point, view_point, plane_point, plane_normal, projection_matrix(), view_matrix());
        }

        /// \brief Project a point from world space onto the viewing plane.
        bool get_view_point(vec2& screen_point, const vec3& world_point) {
            screen_point = vec2(0.0f);
            return true;
        }

        const vec3& position() { return get<Transform>()->translation(); }
        const mat4& view_matrix() { return get<Transform>()->world_inverse(); }
        const mat4& projection_matrix() { return _projection_matrix; }
        const vec3& direction() {
            _direction = normalize(vec3(get<Transform>()->world_matrix() * vec4(0.0f, 0.0f, 1.0f, 0.0f)));
            return _direction;
        }

        unsigned int layer_mask() { return _layer_mask; }
        bool has_layer(unsigned int layer) { return (_layer_mask & (1 << layer)) != 0; }

        template<typename... Layers>
        Camera* set_layers(Layers... layers) {
            _layer_mask = 0;
            add_layer(layers)...;
            return this;
        }

        Camera* add_layer(unsigned int layer) {
            _layer_mask |= (1 << layer);
            return this;
        }

        Camera* remove_layer(unsigned int layer) {
            _layer_mask &= ~(1 << layer);
            return this;
        }

    private:
        unsigned int _layer_mask;
        mat4 _projection_matrix;
        vec3 _direction;
    };
}