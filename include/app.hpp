#pragma once

#include <axolote/engine.hpp>

#define DT_MULTIPLIER 200000

class App : public axolote::Window
{
public:
    bool pause = false;
    bool is_pause_key_press = false;

    void main_loop();
    void process_input(float delta_t = 1.0f);
};
