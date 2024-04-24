#pragma once

#include <axolote/engine.hpp>

class App : public axolote::Window {
public:
    bool pause = false;
    bool is_pause_key_press = false;

    void main_loop(const char *json_filename = "");
    void process_input(float delta_t = 1.0f);
};
