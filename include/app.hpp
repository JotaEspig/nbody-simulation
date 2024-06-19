/**
 * \file app.hpp
 * \brief Main application class
 * \author João Vitor Espig (JotaEspig)
 **/
#pragma once

#include <axolote/engine.hpp>

#include "celestial_body_system.hpp"

/**
 * \brief Main application class
 * \author João Vitor Espig (JotaEspig)
 **/
class App : public axolote::Window {
public:
    /** distance to (0, 0, 0) **/
    float distance = 500.0f;
    /** Latitude **/
    float latitude = 0.0f;
    /** Longitude **/
    float longitude = 0.0f;
    /** Where camera looks **/
    glm::vec3 focus_point = glm::vec3{0.0f, 0.0f, 0.0f};
    /** Is simulation paused **/
    bool pause = false;
    /** Celestial bodies **/
    std::shared_ptr<CelestialBodySystem> bodies_system
        = std::make_shared<CelestialBodySystem>();

    /**
     * \brief renders simulation in real time mode
     * \author João Vitor Espig (JotaEspig)
     * \param json_filename - json filename
     **/
    void main_loop(const char *json_filename = "");
    /**
     * \brief bake simulation
     * \author João Vitor Espig (JotaEspig)
     * \param json_filename - json filename
     **/
    void bake(const char *json_filename);
    /**
     * \brief render baked simulation
     * \author João Vitor Espig (JotaEspig)
     * \param json_filename - json filename
     **/
    void render_loop(const char *json_filename = "");
    /**
     * \brief process input
     * \author João Vitor Espig (JotaEspig)
     * \param delta_t - delta time
     **/
    void process_input(float delta_t = 1.0f);
    /**
     * \brief process especial input used when rendering without baked data
     * \author João Vitor Espig (JotaEspig)
     * \param delta_t - delta time
     **/
    void process_input_real_time_mode(float delta_t = 1.0f);

private:
    /**
     * \brief update focus point
     * \author João Vitor Espig (JotaEspig)
     * \param delta_t - delta time
     **/
    void update_focus_point(float delta_t);
    /**
     * \brief update camera position
     * \author João Vitor Espig (JotaEspig)
     * \param delta_t - delta time
     **/
    void update_camera_position(float delta_t);
};
