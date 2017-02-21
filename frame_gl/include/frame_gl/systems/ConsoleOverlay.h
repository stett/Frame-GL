#pragma once
#include <sstream>
#include <ostream>
#include <unordered_map>
#include <queue>
#include <locale>
#include <iostream>
#include <string>
#include <locale>
#include <codecvt>
#include <iomanip>
#include "frame/System.h"
#include "frame/Log.h"
#include "frame_gl/systems/Input.h"
#include "frame_gl/systems/DebugDraw.h"
#include "frame/Frame.h"

namespace frame_gl
{
    FRAME_SYSTEM(ConsoleOverlay)
    {
    public:
        ConsoleOverlay() : open(false) {}
        ~ConsoleOverlay() {}

    protected:

        void setup() {
            Log::set_output_stream(output_buffer);
        }

        void teardown() {
            Log::reset_output_stream();
        }

        void step() {
            Input* input = system<Input>();

            // Toggle the console
            if (input->key_pressed(GLFW_KEY_GRAVE_ACCENT)) {
                open = !open;
                if (open)
                    input->keyboard_capture(&input_buffer);
                else
                    input->keyboard_release();
            }

            if (open) {

                // Check for the enter key
                if (input->key_pressed(KEY_ENTER)) {

                    // Execute & clear the input buffer
                    input_buffer.flush();
                    std::string command(input_buffer.str());
                    std::transform(command.begin(), command.end(), command.begin(), ::tolower);
                    frame()->commands().run(command);
                    input_buffer = std::stringstream();
                }

                if (input->key_pressed(KEY_BACKSPACE)) {

                    std::string str = input_buffer.str();
                    if (str.size() == 0) return;
                    str.erase(str.end() - 1);
                    input_buffer = std::stringstream(str);
                    input_buffer.seekg(0, std::ios_base::end);
                    input_buffer.seekp(0, std::ios_base::end);
                }

                DebugDraw* debug_draw = system<DebugDraw>();
                if (debug_draw) {

                    {
                        std::string line;
                        output_buffer.flush();

                        while (std::getline(output_buffer, line)) {
                            screen_buffer.push_back(line);
                            if (screen_buffer.size() > 25)
                                screen_buffer.pop_front();
                        }

                        //
                        // TODO: Don't do this, you moron.
                        //       Just do a custom "safe" getline.
                        //
                        // RETORT: Ah, man, but those are so annoying.
                        //         Why the hell doesn't it just work?
                        //
                        output_buffer = std::stringstream();
                        Log::set_output_stream(output_buffer);

                        auto state = output_buffer.rdstate();
                        auto flags = output_buffer.flags();
                        auto good = output_buffer.good();
                    }

                    {
                        glm::vec2 pos(10.0f);
                        for (auto& line : screen_buffer) {
                            debug_draw->screen_text(pos + vec2(1.0f), line, vec4(vec3(0.0f), 1.0f), 16.0f, 3.0f);
                            debug_draw->screen_text(pos, line, vec4(vec3(0.9f), 1.0f), 16.0f, 1.0f);
                            pos.y += 16.0f;
                        }

                        input_buffer.flush();
                        std::string input_string = frame::type_prefixes[frame::LogType::User] + input_buffer.str();
                        debug_draw->screen_text(pos + vec2(1.0f), input_string, vec4(vec3(0.0f), 1.0f), 16.0f, 3.0f);
                        debug_draw->screen_text(pos, input_string, vec4(1.0f, 0.5f, 0.5f, 1.0f), 16.0f, 1.0f);
                    }
                }
            }
        }

    private:
        std::list<std::string> screen_buffer;
        std::stringstream output_buffer;
        std::stringstream input_buffer;
        bool open;
    };
}