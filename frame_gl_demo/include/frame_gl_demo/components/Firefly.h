#pragma once
#include "frame/Component.h"
#include "frame_gl/components/Transform.h"
#include "frame_gl/math.h"
using namespace frame;

namespace frame_demo
{
    FRAME_COMPONENT(Firefly) {
    public:
        struct Influence {
            Influence(float range=1.0f, float strength=1.0f) : range(range), strength(strength) {}
            float range;
            float strength;
            float range_sq() { return range * range; }
        };

    public:
        Firefly() :
            separation(Influence(0.4f, 7.0f)),
            alignment(Influence(4.0f, 4.0f)),
            cohesion(Influence(40.0f, 8.0f)),
            velocity(vec3(0.0f)),
            acceleration(vec3(0.0f)),
            drag(1.5f) {}

    public:
        const vec3& get_velocity() { return velocity; }
        const vec3& get_heading() { return heading; }
        Firefly* set_velocity(const vec3& new_velocity) { velocity = new_velocity; return this; }
        Firefly* add_velocity(const vec3& delta_velocity) { velocity += delta_velocity; return this; }
        Firefly* update_heading() { heading = normalize(velocity); return this; }
        Firefly* set_influences(const Influence& new_separation, const Influence& new_alignment, const Influence& new_cohesion) {
            separation = new_separation;
            alignment = new_alignment;
            cohesion = new_cohesion;
            return this;
        }

    public:
        Influence separation;
        Influence alignment;
        Influence cohesion;
        vec3 heading;
        vec3 velocity;
        vec3 acceleration;
        float drag;
    };
}