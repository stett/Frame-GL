#pragma once
#include <sstream>
#include <unordered_map>
#include <queue>
#include "frame/System.h"
#include "frame/Log.h"
#include "frame_gl/systems/Input.h"
#include "frame/Frame.h"

namespace frame_gl
{
    FRAME_SYSTEM(ConsoleOverlay)
    {
    public:
        ConsoleOverlay(float key_timeout=0.25f) : key_timeout(key_timeout) {}
        ~ConsoleOverlay() {}

    protected:

        void setup() {
        }

        void step() {
            Input* input = system<Input>();

            // Update key timeouts
            std::queue< char > remove;
            for (auto& k : key_timer) {
                if (k.second > 0.0f)
                    k.second -= dt();
                else
                    remove.push(k.first);
            }

            // Remove keys which we can use again
            while (!remove.empty()) {
                key_timer.erase(key_timer.find(remove.front()));
                remove.pop();
            }

            // Check for the enter key
            if (input->key_pressed(KEY_ENTER)) {

                // Execute & clear the input buffer
                input_buffer.flush();
                std::string command(input_buffer.str());
                std::transform(command.begin(), command.end(), command.begin(), ::tolower);
                frame()->commands().run(command);
                input_buffer = std::stringstream();

            } else {

                // Get new input
                bool changed = false;
                auto& keys = input->keyboard_state().keys;
                for (size_t i = 0; i < keys.size(); ++i) {

                    if (i == KEY_ENTER)
                        continue;

                    char c = (char)i;
                    auto it = key_timer.find(c);
                    if (it == key_timer.end()) {
                        if (keys[i]) {
                            input_buffer << c;
                            key_timer.insert(it, std::make_pair(c, key_timeout));
                            changed = true;
                        }
                    }
                }

                // TEMP
                // If input buffer changed, print it
                if (changed)
                    Log::input(input_buffer.str());
                // END TEMP
            }
        }

    private:
        std::stringstream input_buffer;
        std::unordered_map< char, float > key_timer;
        float key_timeout;
    };
}