#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <nlohmann/json.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#include "app.hpp"
#include "celestial_body_system.hpp"

void App::process_input(float delta_t) {
    Window::process_input(delta_t);

    int pause_key_state = glfwGetKey(window, GLFW_KEY_P);
    if (pause_key_state == GLFW_PRESS && !is_pause_key_press) {
        is_pause_key_press = true;
    }
    else if (pause_key_state == GLFW_RELEASE && is_pause_key_press) {
        pause = !pause;
        is_pause_key_press = false;
    }
}

void App::main_loop(const char *json_filename) {
    using json = nlohmann::json;
    std::ifstream file(json_filename);
    json data = json::parse(file);
    double dt_multiplier = data["dt_multiplier"];

    std::string original_title = title();

    axolote::gl::Shader shader_program(
        "./resources/shaders/vertex_shader.glsl",
        "./resources/shaders/fragment_shader.glsl"
    );

    shader_program.activate();
    shader_program.set_uniform_int("light.is_set", 0);

    // Table with planets data:
    // https://nssdc.gsfc.nasa.gov/planetary/factsheet/

    // website used for initial velocity calculation:
    // https://pt.calcprofi.com/calculadora-formula-velocidade-orbital.html
    // using: parâmetros -> km, kg and m/s

    // --------------------------------------------------------------------

    // Celestial Body system
    CelestialBodySystem ss;
    ss.setup_using_json(shader_program, data);

    // Scene object
    current_scene = std::make_shared<axolote::Scene>();
    // Configs camera (points it downwards)
    current_scene->camera.fov = 70.0f;
    current_scene->camera.pos = glm::vec3{0.0f, 300.0f, 0.0f};
    current_scene->camera.orientation
        = glm::normalize(glm::vec3{0.01f, -1.0f, 0.0f});
    current_scene->camera.speed = 80.0f;
    current_scene->camera.sensitivity = 10000.0f;
    // See why this doesnt work
    current_scene->camera.max_dist = 100.0f;

    // Add celestial bodies as drawable to scene
    for (auto &e : ss.celestial_bodies()) {
        current_scene->add_drawable(e);
    }

    double before = glfwGetTime();
    while (!should_close()) {
        glClearColor(_color.r, _color.g, _color.b, _color.opacity);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glfwPollEvents();

        double now = glfwGetTime();
        double dt = now - before;
        before = now;
        process_input(dt);

        std::stringstream sstr;
        sstr << original_title << " | " << (int)(1 / dt) << " fps";
        set_title(sstr.str());

        if (!pause) {
            dt *= dt_multiplier;

            ss.update(dt);

            auto scene = std::make_shared<axolote::Scene>();
            scene->camera = current_scene->camera;

            for (auto &e : ss.celestial_bodies()) {
                scene->add_drawable(e);
            }

            current_scene = scene;
            current_scene->update(dt);
        }

        current_scene->update_camera((float)width() / height());
        current_scene->render();

        glfwSwapBuffers(window);
    }
}
