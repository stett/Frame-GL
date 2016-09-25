#pragma once
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/matrix_interpolation.hpp"
#include "frame/Component.h"
#include "frame/util/ParentChild.h"
#include "frame_gl/math.h"

namespace frame
{
    FRAME_COMPONENT_HIERARCHIC(Transform) {
    private:
        const int MATRIX            = 1 << 0;
        const int INVERSE           = 1 << 1;
        const int WORLD_MATRIX      = 1 << 2;
        const int WORLD_INVERSE     = 1 << 3;
        const int WORLD_TRANSLATION = 1 << 4;
        const int ALL = MATRIX | INVERSE | WORLD_MATRIX | WORLD_INVERSE | WORLD_TRANSLATION;

    public:
        Transform(const vec3& translation=vec3(0.0f), const quat& rotation=quat(), const vec3& scale=vec3(1.0f))
        : _translation(translation), _rotation(rotation), _scale(scale), _valid(0) {}

    public:

        Transform* set_translation(const vec3& translation) {
            _translation = translation;
            invalidate(ALL);
            return this;
        }

        Transform* add_translation(const vec3& translation_delta) {
            _translation += translation_delta;
            invalidate(ALL);
            return this;
        }

        Transform* set_rotation(const quat& rotation) {
            _rotation = rotation;
            invalidate(ALL);
            return this;
        }

        Transform* set_rotation(const vec3& axis, float angle) {
            //
            // TODO: Actually set the quaternion...
            //
            _rotation = glm::angleAxis(angle, axis);
            invalidate(ALL);
            return this;
        }

        Transform* look(const vec3& target, const vec3& up=vec3(0.0f, 1.0f, 0.0f)) {
            if (_translation == target) return this;
            //
            // TODO: Fix this... it DEFINITELY doesn't work.
            //
            _rotation = glm::conjugate(glm::quat_cast(glm::lookAt(world_translation(), target, up)));
            invalidate(ALL);
            return this;
        }

        Transform* set_scale(const vec3& scale) {
            _scale = scale;
            invalidate(ALL);
            return this;
        }

        Transform* set_scale(float scale) {
            return set_scale(vec3(scale));
        }

        const vec3& translation() const { return _translation; }

        const quat& rotation() const { return _rotation; }

        const vec3& scale() const { return _scale; }

        const mat4& matrix() const {
            if (invalid(MATRIX)) {
                validate(MATRIX);
                _matrix = glm::scale(glm::translate(mat4(1.0f), _translation) * glm::toMat4(_rotation), _scale);
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

        //
        // TODO: Make the world_* methods better... way too much math going on in these accessors
        //

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
        */

        const vec3& world_translation() const {
            if (invalid(WORLD_TRANSLATION)) {
                validate(WORLD_TRANSLATION);
                _world_translation =  world_matrix() * vec4(vec3(0.0f), 1.0f);
            }

            return _world_translation;
        }

        /*
        // TODO: Remember how to concatenate quaternions...
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

        /// \brief Returns true if all of the given flags are on
        inline bool valid(int flags) const { return (_valid & flags) == flags; }

        /// \brief Returns true if any of the flags are off
        inline bool invalid(int flags) const { return !valid(flags); }

    private:
        inline void validate(int flags) const {_valid |= flags; }

        inline void invalidate(int flags) {
            _valid &= ~flags;
            for (Transform* child : children())
                child->invalidate(WORLD_MATRIX | WORLD_INVERSE);
        }

    private:
        vec3 _translation;
        quat _rotation;
        vec3 _scale;
        mutable vec3 _world_translation;
        //mutable quat _world_rotation;
        //mutable vec3 _world_scale;
        mutable mat4 _matrix;
        mutable mat4 _inverse;
        mutable mat4 _world_matrix;
        mutable mat4 _world_inverse;
        mutable int _valid;
    };
}