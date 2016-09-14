#pragma once
#include <string>
#include <GLFW/glfw3.h>
#include "frame/config.h"
#include "frame/System.h"
#include "frame/Node.h"
#include "frame/Log.h"
#include "frame/Resource.h"
#include "frame/Frame.h"
#include "frame/Message.h"
#include "frame_gl/components/RenderTarget.h"
#include "frame_gl/data/Shader.h"
#include "frame_gl/data/Mesh.h"
#include "frame_gl/math.h"

namespace frame
{
    FRAME_SYSTEM(Window, Node<RenderTarget>, Node<Frame>) {
    public:
        enum FitMode { Stretch, Fit, Fill, Center };

    public:
        Window(ivec2 size=ivec2(0), bool resizeable=true, FitMode fit_mode=Fit, bool vsync=false);
        ~Window();

    public:
        const ivec2& size() const;
        void set_clear_color(const vec3& clear_color) { this->clear_color = clear_color; };

    public:
        //
        // TODO: This is bad - make a better mechanism!!!
        //
        static Window* get_window(Frame* frame);

    protected:
        virtual void setup();
        virtual void step(float dt);
        virtual void teardown();

    private:
        void calculate_buffer_transform(RenderTarget* buffer, mat4& transform);

    public:
        static void resize_callback(GLFWwindow* window, int width, int height);
        static void keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
        static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
        static void mouse_position_callback(GLFWwindow* window, double x, double y);
        static void mouse_scroll_callback(GLFWwindow* window, double dx, double dy);

    public:
        Message<const ivec2& /*size*/>                                          resize;
        Message<int /*key*/, int /*scancode*/, int /*action*/, int /*mods*/>    keyboard;
        Message<int /*button*/, int /*action*/, int /*mods*/>                   mouse_button;
        Message<const vec2& /*position*/>                                       mouse_position;
        Message<const vec2& /*delta position*/>                                 mouse_scroll;

    private:
        static int& count();

    private:
        GLFWwindow* window;
        ivec2 _size;
        glm::mat4 transform;
        FitMode fit_mode;
        vec3 clear_color;
        bool vsync;
        Mesh* mesh;
        Shader* shader;
    };
}