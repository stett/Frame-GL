#pragma once
#include <vector>
#include <string>
#include <tuple>
#include <typeindex>
#include <unordered_map>
#include "frame/System.h"
#include "frame_gl/components/Camera.h"
#include "frame_gl/components/Transform.h"
#include "frame_gl/components/MeshRenderer.h"
#include "frame_gl/data/Shader.h"
#include "frame_gl/math.h"

namespace frame
{
    /// \class Render
    /// \brief Draws all MeshRenderer components to all Camera target textures.
    FRAME_SYSTEM(Render, Node<RenderTarget>, Node<Camera, RenderTarget>, Node<MeshRenderer>) {

    public:
        struct GlobalPropertyBase {
            virtual void bind(const Shader* shader) = 0;
            virtual std::type_index type() const = 0;
        };

        template <typename T>
        struct GlobalProperty : public GlobalPropertyBase {
            GlobalProperty(const std::string& name, const T& value) : name(name), value(value) {}
            void bind(const Shader* shader) const { shader->uniform(name.c_str(), value); }
            std::type_index type() const { return std::type_index(typeid(T)); }
            std::string name;
            T value;
        };

    public:
        const unsigned int MAX_LAYERS = 32;

    public:
        Render(bool auto_clear=true)
        : display_targets(std::vector<RenderTarget*>(MAX_LAYERS, nullptr)), display_cameras(std::vector<Camera*>(MAX_LAYERS, nullptr)),/*_display_target(nullptr), _display_camera(nullptr), */auto_clear(auto_clear) {}

    public:

        /// \brief Returns a pointer to the Camera, if there is one, which renders to the RenderTarget returned by display_target().
        Camera* display_camera(unsigned int layer=0) {
            return (layer < MAX_LAYERS) ? display_cameras[layer] : nullptr;
        }

        RenderTarget* display_target(unsigned int layer=0) {
            return (layer < MAX_LAYERS) ? display_targets[layer] : nullptr;
        }

        /*
        /// \brief Set a global uniform
        template <typename T>
        void set_global_uniform(const std::string& name, const T& value) {
            auto it = global_uniforms.find(name);

            // Make a new entry
            if (it == global_uniforms.end()) {
                auto property = std::shared_ptr< GlobalProperty< T > >(new GlobalProperty<T>(name, value));
                global_uniforms.insert(it, static_cast< std::shared_ptr< GlobalPropertyBase > >(property));

            // Update the existing entry, if it is of the same type
            } else if (it->second->type() == std::type_index(typeid(T))) {
                it->second->value = value;

            // It's an error if we try to set an existing value to a different type
            } else {
                frame::Log::error("Can't set existing global uniform to a differently typed value!");
            }
        }

        /// \brief Bind all global uniforms to a shader
        void bind_global_uniforms(const Shader* shader) {
            for (auto it : global_uniforms)
                it.second->bind(shader);
        }
        */

    protected:
        virtual void step(float dt);

    private:
        std::vector<RenderTarget*> display_targets;
        std::vector<Camera*> display_cameras;
        //std::unordered_map< std::string, std::shared_ptr< GlobalPropertyBase > > global_uniforms;
        bool auto_clear;
    };
}