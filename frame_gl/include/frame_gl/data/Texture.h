#pragma once
#include "frame_gl/math.h"

namespace frame
{
    class Texture {
    public:
        Texture(const ivec2& size=ivec2(1), bool multisample=true);
        ~Texture();
        Texture(const Texture& other) = delete;
        Texture& operator=(const Texture& other) = delete;

    public:
        const ivec2& size() const { return _size; }
        unsigned int id() const { return _id; }
        bool multisample() const { return _multisample; }
        void bind(unsigned int texture_unit);
        void unbind();

    private:
        ivec2 _size;
        unsigned int _id;
        bool _multisample;

    public:

        struct Preset {
        public:
            //static Resource<Texture> white_pixel() { static Resource<Texture> texture(ivec2(1)); /* TODO: Fill with whiteness */ return texture; }

        private:
            Preset() {}
            ~Preset() {}
        };
    };
}