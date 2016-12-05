#define GLEW_STATIC
#include <vector>
#include <set>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include "frame/Resource.h"
#include "frame_gl/systems/Window.h"
#include "frame_gl/systems/Render.h"
#include "frame_gl/data/Shader.h"
#include "frame_gl/error.h"
#include "frame_gl/math.h"
using namespace frame;

Window::Window(ivec2 size, bool resizeable, FitMode fit_mode, bool vsync, const glm::vec3& clear_color)
: _size(size), fit_mode(fit_mode), vsync(vsync), clear_color(clear_color) {

    // Initialize GLFW, if this is the first window
    if (count() == 0) {
        if (!glfwInit()) {
            Log::error("GL failed to initialize");
            return;
        }
    }

    // Log some data about the GL version
    Log::success("GL initialized: " + std::string(glfwGetVersionString()));

    // If no resolution was given, find the native size of the monitor,
    // and take 80% of it
    if (size.x <= 0 || size.y <= 0) {
        const GLFWvidmode* video_mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        size = ivec2(vec2((float)video_mode->width, (float)video_mode->height) * 0.8f);
    }

    // Create a window
    if (!resizeable) glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    window = glfwCreateWindow(size.x, size.y, project_name.c_str(), 0, 0);
    if (!window) {
        Log::error("Failed to open window");
        return;
    }

    // Attach this frame::Window to this glfw::Window
    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, Window::resize_callback);
    glfwSetKeyCallback(window, Window::keyboard_callback);
    glfwSetMouseButtonCallback(window, Window::mouse_button_callback);
    glfwSetCursorPosCallback(window, Window::mouse_position_callback);
    glfwSetScrollCallback(window, Window::mouse_scroll_callback);

    // Joyfully display our success!!
    Log::success("Window opened");

    // Make it the current context & set callbacks
    glfwMakeContextCurrent(window);
    glfwSwapInterval(vsync ? 1 : 0);

    // Make sure GL is in a healthy state
    gl_check();

    // Initialize GLEW & check for errors
    if (count() == 0) {
        auto status = glewInit();
        if (status != GLEW_OK)
            Log::error("GL Extensions failed to initialize: " + std::string((char*)glewGetErrorString(status)));
        else
            Log::success("GL Extensions initialized: GLEW " + std::string((char*)glewGetString(GLEW_VERSION)));
    }

    // Increment the window count
    ++count();
}

Window::~Window() {

    // Decrement window count
    --count();

    // Destroy the window
    glfwDestroyWindow(window);

    // Shut down GLFW, if this was the last window
    if (count() == 0)
        glfwTerminate();
}

void Window::setup() {

    // Select the window context
    glfwMakeContextCurrent(window);

    // Build the buffer mesh
    mesh = new Mesh();
    mesh->add_position(vec3(-1.0f, -1.0f, 0.0f)); mesh->add_uv(vec2(0.0f, 0.0f));
    mesh->add_position(vec3(-1.0f,  1.0f, 0.0f)); mesh->add_uv(vec2(0.0f, 1.0f));
    mesh->add_position(vec3( 1.0f,  1.0f, 0.0f)); mesh->add_uv(vec2(1.0f, 1.0f));
    mesh->add_position(vec3( 1.0f, -1.0f, 0.0f)); mesh->add_uv(vec2(1.0f, 0.0f));
    mesh->add_triangle(0, 1, 2);
    mesh->add_triangle(0, 2, 3);
    mesh->finalize();

    // Build the final phase shader
    shader = new Shader(
        "Window Buffer Shader",

        Resource<ShaderPart>(ShaderPart::Type::Vertex,
            "#version 330\n                                             "
            "layout(location = 0)in vec3 vert_position;                 "
            "layout(location = 2)in vec2 vert_uv;                       "
            "uniform mat4 transform;                                    "
            "out vec2 frag_uv;                                          "
            "void main() {                                              "
            "    frag_uv       = vert_uv;                               "
            "    gl_Position   = transform * vec4(vert_position, 1.0);  "
            "}                                                          "
        ),

        Resource<ShaderPart>(ShaderPart::Type::Fragment,
            "#version 330\n                                 "
            "in vec2 frag_uv;                               "
            "out vec4 pixel_color;                          "
            "uniform sampler2D texture;                     "
            "void main() {                                  "
            "    pixel_color = texture2D(texture, frag_uv); "
            "}                                              "
        )
    );

    transform_location = shader->locate("transform");
}

void Window::teardown() {

    // Delete the buffer mesh & shader
    delete mesh;
    delete shader;
}

Window* Window::get_window(Frame* frame) {

    // Try to get the Window system for this frame
    Window* window = frame->systems().get<Window>();
    if (window) return window;

    // Try to get the Window system for the parent frame
    FrameInterface* parent = frame->parent();
    if (parent) return get_window(*parent);

    // Couldn't find a window... return null :(
    return nullptr;
}

void Window::step(float dt) {

    // Select the window context
    glfwMakeContextCurrent(window);

    // Set up OpenGL state

    //glEnable(GL_DEPTH_TEST);
    glDisable(GL_DEPTH_TEST);

    //glDisable(GL_BLEND);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDisable(GL_CULL_FACE);
    glDisable(GL_STENCIL_TEST);
    glClearColor(clear_color.r, clear_color.g, clear_color.b, 1.0f);

    //glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    gl_check();

    // Initialize a list to contain all display buffers
    std::set<RenderTarget*> buffers;
    //Render* render;
    //RenderTarget* buffer;

    /*
    // If we this frame has a display buffer, add it to the list of buffers to draw to screen
    if ((render = frame()->systems().get<Render>()) && (buffer = render->display_target()))
        buffers.insert(buffer);
    */
    // Build a list of all display targets from this frame
    for (auto buffer : node<RenderTarget>())
        if (buffer->display_layer() != -1)
            buffers.insert(buffer);

    /*
    // Add the display buffers from all subframes
    for (Frame *f : node<Frame>())
        if ((render = f->systems().get<Render>()) && (buffer = render->display_target()))
            buffers.insert(buffer);
    */

    // Only bother setting up stuff if we found some buffers
    if (buffers.size() > 0) {

        // Set the viewport
        glViewport(0, 0, size().x, size().y);

        // Set up the final-pass shader
        shader->bind();

        // Render each of the buffers to the screen
        for (RenderTarget* buffer : buffers) {

            // Find the transform to use for the drawing of this buffer and give it to the shader
            calculate_buffer_transform(buffer, transform);
            shader->uniform(transform_location, transform);

            // Bind the display buffer's texture for reading, and render it to the screen buffer
            buffer->bind_texture(0);
            mesh->render();
            buffer->unbind_texture();
        }

        // We are done with this shader now
        shader->unbind();

        // Finally, blit to the screen
        glfwSwapBuffers(window);

        gl_check();
    }

    // Check for window close request
    //
    // TODO: Stopping the frame on window-removal should be optional!
    //
    glfwPollEvents();
    if (glfwWindowShouldClose(window))
        frame().stop();

    gl_check();
}

void Window::calculate_buffer_transform(RenderTarget* buffer, mat4& transform) {
    //
    // TODO: Update the transform based on the render buffer's ratio,
    //       the screen size ratio, and the fill mode.
    //
    // TODO: Move this into resize somehow... doesn't need to change
    //       every freaking step... Unless, I suppose, the render target
    //       changes size.
    //
    if (fit_mode == Stretch) {

        // No transform is necessary to stretch the buffer to fill the screen.
        transform = mat4(1);

    } else if (fit_mode == Fit) {

        // Set up a scaling transform which will always fit the full buffer
        // on the screen without aspect ratio distortion.
        vec2 scale(vec2(buffer->size()) / vec2(size()));
        transform = glm::scale(mat4(1), vec3(scale / max(scale.x, scale.y), 1.0f));

    } else if (fit_mode == Fill) {

        // Expand the frame buffer until it fills the screen, without aspect
        // ratio distortion, but with clipping! Probably not that great for
        // most purposes.
        vec2 scale(vec2(buffer->size()) / vec2(size()));
        transform = glm::scale(mat4(1), vec3(scale / min(scale.x, scale.y), 1.0f));

    } else if (fit_mode == Center) {

        // Pretty much useless for anything, I think.
        transform = glm::scale(mat4(1), vec3(vec2(buffer->size()) / vec2(size()), 1.0f));
    }
}

void Window::resize_callback(GLFWwindow* window, int width, int height) {
    Window* w = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
    w->_size.x = width;
    w->_size.y = height;
    w->resize(w->_size);
}

void Window::keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    reinterpret_cast<Window*>(glfwGetWindowUserPointer(window))
        ->keyboard(key, scancode, action, mods);
}

void Window::mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    reinterpret_cast<Window*>(glfwGetWindowUserPointer(window))
        ->mouse_button(button, action, mods);
}

void Window::mouse_position_callback(GLFWwindow* glfw_window, double x, double y) {
    Window* window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(glfw_window));

    // Scale the position to the main buffer
    //
    // TODO: Add a better way to select the "main" buffer!
    // Need something better than "just pick the first one".
    //
    vec2 pos(x, y);
    vec2 window_size(window->size());
    for (auto buffer : window->node<RenderTarget>()) {
        if (buffer->display_layer() != -1) {
            mat4 transform;
            window->calculate_buffer_transform(buffer, transform);
            vec2 buffer_size(transform * vec4(window->size(), 0.0f, 0.0f));
            vec2 ratio(window_size / buffer_size);
            vec2 edges(window_size - buffer_size);
            pos = (pos - 0.5f * edges) * ratio;
            break;
        }
    }

    window->mouse_position(pos);
}

void Window::mouse_scroll_callback(GLFWwindow* window, double dx, double dy) {
    reinterpret_cast<Window*>(glfwGetWindowUserPointer(window))
        ->mouse_scroll(vec2(dx, dy));
}

int& Window::count() {
    static int _count = 0;
    return _count;
}
