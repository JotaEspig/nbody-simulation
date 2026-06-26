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
     * \param use_grav_grid - use gravitational grid
     **/
    void main_loop(const char *json_filename, bool use_grav_grid = false);
    /**
     * @brief benchmarks the algorithms
     *
     * @param json_filename - json filename
     * @param duration - duration in seconds
     */
    void benchmark(
        const char *json_filename,
        std::chrono::seconds duration = std::chrono::seconds(10)
    );
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
    void process_input();
    /**
     * \brief process especial input used when rendering without baked data
     * \author João Vitor Espig (JotaEspig)
     * \param delta_t - delta time
     **/
    void process_input_real_time_mode();

private:
    struct BenchmarkEntry {
        CelestialBodySystem::SimulationAlgorithm algorithm;
        const char *name;
    };
    /**
     * \brief update focus point
     * \author João Vitor Espig (JotaEspig)
     **/
    void update_focus_point();
    /**
     * \brief update camera position
     * \author João Vitor Espig (JotaEspig)
     **/
    void update_camera_position();
};
