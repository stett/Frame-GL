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
        enum Type { Perspective, Orthographic };
        struct Settings {
            float vertical_fov;
            float aspect_ratio;
            float clip_near;
            float clip_far;
            vec2 ortho_topleft;
            vec2 ortho_bottomright;
        };

    public:
        Camera(unsigned int layer=0, float vertical_fov=120.0f, float aspect_ratio=1.0f, float clip_near=0.1f, float clip_far=2000.0f)
        : _type(Perspective), _layer_mask(1 << layer), _settings({ vertical_fov, aspect_ratio, clip_near, clip_far, vec2(0.0f), vec2(0.0f) }) { update_matrix(); }
        Camera(unsigned int layer, const vec2& ortho_size, float clip_near=0.0f, float clip_far=2000.0f)
        : _type(Orthographic), _layer_mask(1 << layer), _settings({ 0.0f, 0.0f, clip_near, clip_far, vec2(-ortho_size * 0.5f), vec2(ortho_size * 0.5f) }) { update_matrix(); }
        Camera(unsigned int layer, const vec2& ortho_topleft, const vec2& ortho_bottomright, float clip_near=0.0f, float clip_far=2000.0f)
        : _type(Orthographic), _layer_mask(1 << layer), _settings({ 0.0f, 0.0f, clip_near, clip_far, ortho_topleft, ortho_bottomright }) { update_matrix(); }

    public:

        Camera* set_settings(const Settings& settings) {
            _settings = settings;
            update_matrix();
            return this;
        }

        void resize(const ivec2& size) {
            if (_type == Perspective) {
                _settings.aspect_ratio = float(size.x) / float(size.y);
            } else if (_type == Orthographic) {
                _settings.ortho_topleft = -vec2(size) * 0.5f;
                _settings.ortho_bottomright = vec2(size) * 0.5f;
            }
            update_matrix();
        }

        Camera* bind_target(bool clear=false) { get<RenderTarget>()->bind_target(clear); return this; }

        Camera* unbind_target() { get<RenderTarget>()->unbind_target(); return this; }

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

        const Settings& settings() { return _settings; }
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

        void update_matrix() {
            if (_type == Perspective)
                _projection_matrix = glm::perspective(
                    _settings.vertical_fov,
                    _settings.aspect_ratio,
                    _settings.clip_near,
                    _settings.clip_far);

            else if (_type == Orthographic)
                _projection_matrix = glm::ortho(
                    _settings.ortho_topleft.x, _settings.ortho_bottomright.x,
                    _settings.ortho_bottomright.y, _settings.ortho_topleft.y,
                    _settings.clip_near, _settings.clip_far);
        }

    private:
        Type _type;
        Settings _settings;
        unsigned int _layer_mask;
        mat4 _projection_matrix;
        vec3 _direction;
    };
}