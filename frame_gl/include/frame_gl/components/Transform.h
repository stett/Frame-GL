#pragma once
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/matrix_interpolation.hpp"
#include "frame/Component.h"
#include "frame/util/ParentChild.h"
#include "frame_gl/math.h"
#include "frame_gl/tween.h"

namespace frame
{
    /// \struct TransformElements
    struct TransformElements {
        vec3 translation;
        quat rotation;
        vec3 scale;

        void matrix(mat4& m) const {
            m = glm::scale(glm::translate(mat4(1.0f), translation) * rotation.matrix(), scale);
        }

        void lerp(const TransformElements& a, const TransformElements& b, float t) {
            tween::linear(translation, a.translation, b.translation, t);
            tween::spherical(rotation, a.rotation, b.rotation, t);
            tween::linear(scale, a.scale, b.scale, t);
        }
    };

    /// \class Transform
    /// \brief Hierarchical component which contains a local 3D position and orientation,
    ///        and provides access to it's global data as well.
    FRAME_COMPONENT(Transform), public ParentChild<Transform> {
    private:
        const int MATRIX            = 1 << 0;
        const int INVERSE           = 1 << 1;
        const int WORLD_MATRIX      = 1 << 2;
        const int WORLD_INVERSE     = 1 << 3;
        const int WORLD_TRANSLATION = 1 << 4;
        const int WORLD_ROTATION    = 1 << 5;
        const int WORLD_SCALE       = 1 << 6;
        const int ALL_WORLD = WORLD_MATRIX | WORLD_INVERSE | WORLD_TRANSLATION | WORLD_ROTATION | WORLD_SCALE;
        const int ALL = MATRIX | INVERSE | ALL_WORLD;

    public:
        Transform(const vec3& translation=vec3(0.0f), const quat& rotation=quat(), const vec3& scale=vec3(1.0f))
            : local({ translation, rotation, scale }), _valid(0) {}

    public:

        Transform* set_translation(const vec3& translation) {
            local.translation = translation;
            invalidate(ALL);
            return this;
        }

        Transform* add_translation(const vec3& translation_delta) {
            local.translation += translation_delta;
            invalidate(ALL);
            return this;
        }

        Transform* set_rotation(const quat& rotation) {
            local.rotation = rotation;
            invalidate(ALL);
            return this;
        }

        Transform* set_rotation(const mat4& rotation) {
            local.rotation = quat(rotation);
            invalidate(ALL);
            return this;
        }

        Transform* set_rotation(const vec3& axis, float angle) {
            //
            // TODO: Actually set the quaternion...
            //
            local.rotation = glm::angleAxis(angle, axis);
            invalidate(ALL);
            return this;
        }

        Transform* set_elements(const TransformElements& elements) {
            local = elements;
            invalidate(ALL);
            return this;
        }

        Transform* add_rotation(const mat4& rotation_delta) {
            local.rotation = quat(local.rotation.matrix() + rotation_delta);
            invalidate(ALL);
            return this;
        }

        Transform* add_rotation(const quat& rotation_delta) {
            local.rotation += rotation_delta;
            local.rotation.normalize();
            invalidate(ALL);
            return this;
        }

        Transform* add_rotation(const vec3& rotation_delta) {
            return add_rotation(quat::axis_angle(rotation_delta) * local.rotation);
        }

        Transform* look(const vec3& target, const vec3& up=vec3(0.0f, 1.0f, 0.0f)) {
            if (local.translation == target) return this;
            //
            // TODO: Fix this... it DEFINITELY doesn't work.
            //
            local.rotation = glm::conjugate(glm::quat_cast(glm::lookAt(world_translation(), target, up)));
            invalidate(ALL);
            return this;
        }

        Transform* set_scale(const vec3& scale) {
            local.scale = scale;
            invalidate(ALL);
            return this;
        }

        Transform* set_scale(float scale) {
            return set_scale(vec3(scale));
        }

        const vec3& translation() const { return local.translation; }

        const quat& rotation() const { return local.rotation; }

        const vec3& scale() const { return local.scale; }

        const mat4& matrix() const {
            if (invalid(MATRIX)) {
                validate(MATRIX);
                local.matrix(_matrix);
            }
            return _matrix;
        }

        const mat4& inverse() const {
            if (invalid(INVERSE)) {
                validate(INVERSE);
                _inverse = glm::inverse(matrix());
            }
            return _inverse;
        }

        const TransformElements& elements() const {
            return local;
        }

        /*
        Transform* set_world_translation(const vec3& translation) {
            _translation = translation;
            //
            // TODO: Diff with the parent transformed translation!!!
            //       Important shit, man!
            //
            update();
            return this;
        }

        Transform* set_world_elements(const TransformElements& elements) {
            //
            // TODO: Uh... do this please.
            //
            return this;
        }
        */

        const vec3& world_translation() const {
            if (invalid(WORLD_TRANSLATION)) {
                validate(WORLD_TRANSLATION);
                world.translation =  world_matrix() * vec4(vec3(0.0f), 1.0f);
            }
            return world.translation;
        }

        /*
        // TODO: Remember how to do this stuff...
        const quat& world_rotation() const {
            _world_rotation = parent() ? parent()->world_rotation() * rotation() : rotation();
            return _world_rotation;
        }

        // TODO: Make sure this actually does what I want...
        const vec3& world_scale() const {
            _world_scale = parent() ? parent()->world_matrix() * vec4(scale(), 0.0f) : scale();
            return _world_scale;
        }
        */

        const mat4& world_matrix() const {
            if (invalid(WORLD_MATRIX)) {
                validate(WORLD_MATRIX);
                _world_matrix = parent() ? parent()->world_matrix() * matrix() : matrix();
            }
            return _world_matrix;
        }

        const mat4& world_inverse() const {
            if (invalid(WORLD_INVERSE)) {
                validate(WORLD_INVERSE);
                _world_inverse = glm::inverse(world_matrix());
            }
            return _world_inverse;
        }

        const TransformElements& world_elements() const {
            return world;
        }

        /// \brief Returns true if all of the given flags are on
        inline bool valid(int flags) const { return (_valid & flags) == flags; }

        /// \brief Returns true if any of the flags are off
        inline bool invalid(int flags) const { return !valid(flags); }

    private:
        inline void validate(int flags) const {_valid |= flags; }

        inline void invalidate(int flags) {
            _valid &= ~flags;
            for (Transform* child : children())
                child->invalidate(ALL_WORLD);
        }

    private:
        TransformElements local;
        mutable TransformElements world;
        mutable mat4 _matrix;
        mutable mat4 _inverse;
        mutable mat4 _world_matrix;
        mutable mat4 _world_inverse;
        mutable int _valid;
    };
}