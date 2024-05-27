#include <cstddef>
#include <fstream>
#include <iomanip>
#include <ios>
#include <iostream>
#include <limits>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <nlohmann/json.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#include "app.hpp"
#include "celestial_body_system.hpp"

#define UNUSED(x) (void)(x)

struct BodyDataJSON {
    double mass;
    float pos_x, pos_y, pos_z;
};

void to_json(nlohmann::json &j, const BodyDataJSON &body_data) {
    // use abbreviations to minimize file size
    std::stringstream ssx;
    ssx << std::fixed << std::setprecision(3) << body_data.pos_x;
    std::stringstream ssy;
    ssy << std::fixed << std::setprecision(3) << body_data.pos_y;
    std::stringstream ssz;
    ssz << std::fixed << std::setprecision(3) << body_data.pos_z;
    j = {
        {"m", body_data.mass},
        {"px", ssx.str()},
        {"py", ssy.str()},
        {"pz", ssz.str()},
    };
}

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

void App::process_input_real_time_mode(float delta_t) {
    UNUSED(delta_t);
    bool mouse1_key_state = glfwGetKey(window, GLFW_KEY_X);
    if (mouse1_key_state == GLFW_PRESS && !is_throw_obj_key_press) {
        // Gets intersection between plan and line
        // (Hardcoded plan, a different normal vector wouldn't work)
        // plan : y = 0
        // line : initial point + t * vec_dir
        /*
        glm::vec3 initial_point = current_scene->camera.pos;
        glm::vec3 vec_dir = current_scene->camera.orientation;
        float t = -initial_point.y / vec_dir.y;
        float x = initial_point.x + t * vec_dir.x;
        float z = initial_point.z + t * vec_dir.z;
        glm::vec3 pos{x, 0.0f, z};
        glm::vec3 vel{vec_dir.x / 40000, 0.0f, vec_dir.z / 40000}; */

        glm::vec3 pos = current_scene->camera.pos;
        glm::vec3 vel = (1.0f / 40000) * current_scene->camera.orientation;

        bodies_system.add_celestial_body(100, pos, vel, shader_program);

        is_throw_obj_key_press = true;
    }
    else if (mouse1_key_state == GLFW_RELEASE && is_throw_obj_key_press) {
        is_throw_obj_key_press = false;
    }
}

void App::main_loop(const char *json_filename) {
    using json = nlohmann::json;
    std::ifstream file(json_filename);
    json data = json::parse(file);
    double dt_multiplier = data["dt_multiplier"];

    std::string original_title = title();

    shader_program = axolote::gl::Shader(
        "./resources/shaders/vertex_shader.glsl",
        "./resources/shaders/fragment_shader.glsl"
    );

    shader_program.activate();
    shader_program.set_uniform_int("light.is_set", 0);

    // Celestial Body system
    bodies_system.setup_using_json(shader_program, data);

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
    current_scene->camera.max_dist = 1000.0f;

    // Add celestial bodies as drawable to scene
    for (auto &e : bodies_system.celestial_bodies()) {
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
        process_input_real_time_mode(dt);

        std::stringstream sstr;
        sstr << original_title << " | " << (int)(1 / dt) << " fps";
        set_title(sstr.str());

        if (!pause) {
            dt *= dt_multiplier;

            bodies_system.update(dt);

            auto scene = std::make_shared<axolote::Scene>();
            scene->camera = current_scene->camera;

            for (auto &e : bodies_system.celestial_bodies()) {
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

void App::bake(const char *json_filename) {
    using json = nlohmann::json;
    std::ifstream file(json_filename);
    json data = json::parse(file);
    double dt_multiplier = data["dt_multiplier"];

    // The shader is create just because it's needed in setup_using_json
    axolote::gl::Shader shader_program(
        "./resources/shaders/vertex_shader.glsl",
        "./resources/shaders/fragment_shader.glsl"
    );
    // Current scene is needed for process input from user
    current_scene = std::make_shared<axolote::Scene>();

    // Celestial Body system
    CelestialBodySystem ss;
    ss.setup_using_json(shader_program, data);

    std::cout << "LET HIM COOK!" << std::endl
              << "DO NOT PRESS Ctrl+C" << std::endl;

    std::string s = std::string{json_filename} + ".baked";
    std::ofstream outputfile{s};
    std::size_t counter = 0;
    outputfile << "[" << std::endl;
    while (!should_close()) {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwPollEvents();

        double dt = 1.0 / 60;
        dt *= dt_multiplier;

        process_input(dt);

        ss.update(dt);

        int i = 0;
        int size = ss.celestial_bodies().size();
        outputfile << "[";
        for (auto &c : ss.celestial_bodies()) {
            BodyDataJSON b = {c->mass(), c->pos.x, c->pos.y, c->pos.z};
            json outputjson = b;
            outputfile << outputjson;
            if (i < size - 1) {
                outputfile << ",";
            }
            ++i;
        }
        outputfile << "]";

        ++counter;
        if (counter % 60 == 0) {
            std::cout << "Rendered: " << counter / 60
                      << " seconds --- DO NOT PRESS Ctrl+C" << std::endl;
        }

        if (!should_close()) {
            outputfile << ",";
        }
        outputfile << std::endl;

        glfwSwapBuffers(window);
    }

    outputfile << "]" << std::endl;
}

void App::render_loop(const char *json_filename) {
    using json = nlohmann::json;

    pause = true;
    std::string original_title = title();

    axolote::gl::Shader shader_program(
        "./resources/shaders/vertex_shader.glsl",
        "./resources/shaders/fragment_shader.glsl"
    );

    shader_program.activate();
    shader_program.set_uniform_int("light.is_set", 0);

    // Celestial Body system
    CelestialBodySystem ss;

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
    current_scene->camera.max_dist = 1000.0f;

    std::ifstream file(json_filename);
    std::string line;
    file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    double before = glfwGetTime();
    while (!file.eof() && !should_close()) {
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
            std::getline(file, line);
            if (line == "]") {
                pause = true;
                continue;
            }
            else if (line.back() == ',') {
                line.pop_back();
            }

            json j = json::parse(line);

            auto scene = std::make_shared<axolote::Scene>();
            scene->camera = current_scene->camera;

            ss.setup_using_baked_frame_json(shader_program, j);
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
