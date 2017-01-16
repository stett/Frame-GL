#pragma once
#include "frame/System.h"
#include "frame/Node.h"
#include "frame/Log.h"
#include "frame/Task.h"
#include "frame_gl/math.h"
#include "frame_gl/systems/Input.h"
#include "frame_gl/components/Transform.h"
#include "frame_gl_demo/components/Firefly.h"
using namespace frame;

namespace frame_demo
{
    FRAME_TASK(UpdateFlock) {
    public:
        UpdateFlock() {}
        UpdateFlock(float dt, Entity* entity, Firefly* firefly, Transform* transform, const Node<Firefly, Transform>* fireflies) : dt(dt), entity(entity), firefly(firefly), transform(transform), fireflies(fireflies) {}

    protected:
        void run() {
            vec3 average_separation_position;
            vec3 average_heading;
            vec3 average_cohesion_position;
            float average_separation_count = 0.0f;
            float average_heading_count = 0.0f;
            float average_cohesion_count = 0.0f;

            for (auto f2 : *fireflies) {
                if (entity == f2.entity())
                    continue;

                // Get the distance between these fireflies
                float distance_sq = length2(f2.get<Transform>()->translation() - transform->translation());

                if (distance_sq < firefly->separation.range_sq()) {
                    average_separation_position += f2.get<Transform>()->translation();
                    average_separation_count += 1.0f;
                }

                if (distance_sq < firefly->alignment.range_sq()) {
                    if (length2(f2.get<Firefly>()->get_velocity()) > 0.0f) {
                        average_heading += f2.get<Firefly>()->get_heading();
                        average_heading_count += 1.0f;
                    }
                }

                if (distance_sq < firefly->cohesion.range_sq()) {
                    average_cohesion_position = f2.get<Transform>()->translation();
                    average_cohesion_count += 1.0f;
                }
            }

            // Divide sums by counts to get averages
            if (average_separation_count > 0.0f)    average_separation_position /= average_separation_count;
            if (average_heading_count > 0.0f)       average_heading             /= average_heading_count;
            if (average_cohesion_count > 0.0f)      average_cohesion_position   /= average_cohesion_count;

            // Compute new acceleration
            firefly->acceleration = 
                firefly->separation.strength * normalize(transform->translation() - average_separation_position) +
                firefly->alignment.strength * average_heading +
                firefly->cohesion.strength * normalize(average_cohesion_position - transform->translation());

            // Add some drag
            firefly->acceleration += -firefly->drag * firefly->velocity;
        }

    private:
        float dt;
        Entity* entity;
        Firefly* firefly;
        Transform* transform;
        const Node<Firefly, Transform>* fireflies;
    };


    FRAME_TASK(UpdateVelocity, UpdateFlock) {
    public:
        UpdateVelocity() {}
        UpdateVelocity(float dt, Firefly* firefly) : dt(dt), firefly(firefly) {}

    protected:
        void run() {
            firefly->velocity += firefly->acceleration * dt;
            firefly->update_heading();
        }

    private:
        float dt;
        Firefly* firefly;
    };


    FRAME_TASK(UpdatePosition, UpdateVelocity) {
    public:
        UpdatePosition() {}
        UpdatePosition(float dt, Firefly* firefly, Transform* transform) : dt(dt), firefly(firefly), transform(transform) {}

    protected:
        void run() {
            transform->add_translation(firefly->velocity * dt);
            transform->set_rotation(rotation_between(vec3(1.0f, 0.0f, 0.0f), normalize(firefly->velocity)));
        }

    private:
        float dt;
        Firefly* firefly;
        Transform* transform;
    };


    FRAME_SYSTEM(FireflyOperator, Node<Firefly, Transform>) {
    public:
        FireflyOperator() : concurrent(true) {}

    private:
        bool concurrent;

    public:
        void step() {

            // Toggle concurrency mode
            auto input = frame()->systems().get<Input>();
            if (input && input->key_pressed(' ')) {
                concurrent = !concurrent;
                Log::write(std::string("Concurrent flocking: ") + (concurrent ? "on" : "off"));
            }
        }

        void step_fixed() {

            if (concurrent) {

                for (auto f : node<Firefly, Transform>()) {
                    enqueue<UpdateFlock>(dt_fixed(), f.entity(), f.get<Firefly>(), f.get<Transform>(), &node<Firefly, Transform>());
                    enqueue<UpdateVelocity>(dt_fixed(), f.get<Firefly>());
                    enqueue<UpdatePosition>(dt_fixed(), f.get<Firefly>(), f.get<Transform>());
                }

            } else {

                // Update flocking velocities
                for (auto f1 : node<Firefly, Transform>()) {

                    vec3 average_separation_position;
                    vec3 average_heading;
                    vec3 average_cohesion_position;
                    float average_separation_count = 0.0f;
                    float average_heading_count = 0.0f;
                    float average_cohesion_count = 0.0f;

                    for (auto f2 : node<Firefly, Transform>()) {
                        if (f1.entity() == f2.entity())
                            continue;

                        // Get the distance between these fireflies
                        float distance_sq = length2(f2.get<Transform>()->translation() - f1.get<Transform>()->translation());

                        if (distance_sq < f1.get<Firefly>()->separation.range_sq()) {
                            average_separation_position += f2.get<Transform>()->translation();
                            average_separation_count += 1.0f;
                        }

                        if (distance_sq < f1.get<Firefly>()->alignment.range_sq()) {
                            if (length2(f2.get<Firefly>()->get_velocity()) > 0.0f) {
                                average_heading += f2.get<Firefly>()->get_heading();
                                average_heading_count += 1.0f;
                            }
                        }

                        if (distance_sq < f1.get<Firefly>()->cohesion.range_sq()) {
                            average_cohesion_position = f2.get<Transform>()->translation();
                            average_cohesion_count += 1.0f;
                        }
                    }

                    // Divide sums by counts to get averages
                    if (average_separation_count > 0.0f)    average_separation_position /= average_separation_count;
                    if (average_heading_count > 0.0f)       average_heading             /= average_heading_count;
                    if (average_cohesion_count > 0.0f)      average_cohesion_position   /= average_cohesion_count;

                    // Compute new acceleration
                    f1.get<Firefly>()->acceleration = 
                        f1.get<Firefly>()->separation.strength * normalize(f1.get<Transform>()->translation() - average_separation_position) +
                        f1.get<Firefly>()->alignment.strength * average_heading +
                        f1.get<Firefly>()->cohesion.strength * normalize(average_cohesion_position - f1.get<Transform>()->translation());

                    // Add some drag
                    f1.get<Firefly>()->acceleration += -f1.get<Firefly>()->drag * f1.get<Firefly>()->velocity;
                }

                // Update velocities
                for (auto f : node<Firefly, Transform>()) {
                    f.get<Firefly>()->velocity += f.get<Firefly>()->acceleration * dt_fixed();
                    f.get<Firefly>()->update_heading();
                }

                // Update positions & angles
                for (auto f : node<Firefly, Transform>()) {
                    f.get<Transform>()->add_translation( f.get<Firefly>()->velocity * dt_fixed());
                    f.get<Transform>()->set_rotation(rotation_between(vec3(1.0f, 0.0f, 0.0f), normalize(f.get<Firefly>()->velocity)));
                }
            }
        }
    };
}