#pragma once

#include <axolote/engine.hpp>

#include "celestial_body_system.hpp"

class App : public axolote::Window {
public:
    bool pause = false;
    bool is_pause_key_press = false;
    bool is_throw_obj_key_press = false;
    CelestialBodySystem bodies_system;
    axolote::gl::Shader shader_program;

    void main_loop(const char *json_filename = "");
    void bake(const char *json_filename);
    void render_loop(const char *json_filename = "");
    void process_input(float delta_t = 1.0f);
    void process_input_real_time_mode(float delta_t = 1.0f);
};
