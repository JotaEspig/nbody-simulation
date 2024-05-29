#pragma once

#include <axolote/engine.hpp>

#include "celestial_body_system.hpp"

class App : public axolote::Window {
public:
    bool pause = false;
    std::shared_ptr<CelestialBodySystem> bodies_system = std::make_shared<CelestialBodySystem>();
    axolote::gl::Shader shader_program;

    void main_loop(const char *json_filename = "");
    void bake(const char *json_filename);
    void render_loop(const char *json_filename = "");
    void process_input(float delta_t = 1.0f);
    void process_input_real_time_mode(float delta_t = 1.0f);
};
