#pragma once
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/matrix_interpolation.hpp"
#include "frame/Component.h"
#include "frame/util/ParentChild.h"
#include "frame_gl/math.h"

namespace frame
{
    FRAME_COMPONENT_HIERARCHIC(Transform) {
    public:
        Transform(const vec3& translation=vec3(0.0f), const quat& rotation=quat(), const vec3& scale=vec3(1.0f)) : _translation(translation), _rotation(rotation), _scale(scale) { update(); }

    public:

        Transform* set_translation(const vec3& translation) {
            _translation = translation;
            update();
            return this;
        }

        Transform* add_translation(const vec3& translation_delta) {
            _translation += translation_delta;
            update();
            return this;
        }

        Transform* set_rotation(const quat& rotation) {
            _rotation = rotation;
            update();
            return this;
        }

        Transform* set_rotation(const vec3& axis, float angle) {
            //
            // TODO: Actually set the quaternion...
            //
            _rotation = glm::angleAxis(angle, axis);
            update();
            return this;
        }

        Transform* look(const vec3& target, const vec3& up=vec3(0.0f, 1.0f, 0.0f)) {
            if (_translation == target) return this;
            _matrix = glm::lookAt(_translation, target, up);
            _inverse = glm::inverse(_matrix);

            //
            // TODO: Fix this... it DEFINITELY doesn't work.
            //
            _rotation = quat(_matrix);

            return this;
        }

        Transform* set_scale(const vec3& scale) {
            _scale = scale;
            update();
            return this;
        }

        Transform* set_scale(float scale) {
            set_scale(vec3(scale));
            return this;
        }

        const vec3& translation() const { return _translation; }

        const quat& rotation() const { return _rotation; }

        const vec3& scale() const { return _scale; }

        const mat4& matrix() const { return _matrix; }

        const mat4& inverse() const { return _inverse; }

        //
        // TODO: Make the world_* methods better... way too much math going on in these accessors
        //

        Transform* set_world_translation(const vec3& translation) {
            _translation = translation;
            //
            // TODO: Diff with the parent transformed translation!!!
            //       Important shit, man!
            //
            update();
            return this;
        }

        const vec3& world_translation() { _world_translation = parent() ? parent()->world_translation() + translation() : translation(); return _world_translation; }

        // TODO: Remember how to concatenate quaternions...
        const quat& world_rotation() { _world_rotation = parent() ? parent()->world_rotation() * rotation() : rotation(); return _world_rotation; }

        const vec3& world_scale() { _world_scale = parent() ? parent()->world_scale() * scale() : scale(); return _world_scale; }

        const mat4& world_matrix() { _world_matrix = parent() ? parent()->world_matrix() * matrix() : matrix(); return _world_matrix; }

        const mat4& world_inverse() { _world_inverse = glm::inverse(world_matrix()); return _world_inverse; }

    private:
        void update() {
            _matrix = glm::translate(mat4(1.0f), _translation) * glm::scale(glm::mat4_cast(_rotation), _scale);
            _inverse = glm::inverse(_matrix);
        }

    private:
        vec3 _translation;
        quat _rotation;
        vec3 _scale;
        mat4 _matrix;
        mat4 _inverse;
        vec3 _world_translation;
        quat _world_rotation;
        vec3 _world_scale;
        mat4 _world_matrix;
        mat4 _world_inverse;
    };
}