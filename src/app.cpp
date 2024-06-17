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
    KeyState pause_key_state = get_key_state(Key::P);
    if (pause_key_state == KeyState::PRESSED && !is_key_pressed(Key::P)) {
        set_key_pressed(Key::P, true);
    }
    else if (pause_key_state == KeyState::RELEASED && is_key_pressed(Key::P)) {
        pause = !pause;
        set_key_pressed(Key::P, false);
    }

    if (get_key_state(Key::ESCAPE) == KeyState::PRESSED) {
        set_should_close(true);
    }

    /** Constant multiplied when distance is modified my camera movement,
     * that's because it seems slower than latitude and longitude movements **/
    float distance_modifier = 2.0f;
    if (get_key_state(Key::W) == KeyState::PRESSED) {
        distance -= distance_modifier * current_scene()->camera.speed * delta_t;
    }
    if (get_key_state(Key::A) == KeyState::PRESSED) {
        longitude += current_scene()->camera.speed * delta_t;
    }
    if (get_key_state(Key::S) == KeyState::PRESSED) {
        distance += distance_modifier * current_scene()->camera.speed * delta_t;
    }
    if (get_key_state(Key::D) == KeyState::PRESSED) {
        longitude -= current_scene()->camera.speed * delta_t;
    }
    if (get_key_state(Key::SPACE) == KeyState::PRESSED) {
        latitude += current_scene()->camera.speed * delta_t;
    }
    if (get_key_state(Key::LEFT_SHIFT) == KeyState::PRESSED) {
        latitude -= current_scene()->camera.speed * delta_t;
    }

    // Update camera position according to latitude, longitude and distance and
    // camera must be looking at (0, 0, 0)
    current_scene()->camera.pos.x
        = distance * cos(glm::radians(latitude)) * cos(glm::radians(longitude));
    current_scene()->camera.pos.y = distance * sin(glm::radians(latitude));
    current_scene()->camera.pos.z
        = distance * cos(glm::radians(latitude)) * sin(glm::radians(longitude));
    current_scene()->camera.orientation
        = glm::normalize(-current_scene()->camera.pos);
}

void App::process_input_real_time_mode(float delta_t) {
    UNUSED(delta_t);
    KeyState x_key_state = get_key_state(Key::X);
    if (x_key_state == KeyState::PRESSED && !is_key_pressed(Key::X)) {
        glm::vec3 pos = current_scene()->camera.pos;
        glm::vec3 vel = (1.0f / 40000) * current_scene()->camera.orientation;

        bodies_system->add_body(100, pos, vel);

        set_key_pressed(Key::X, true);
    }
    else if (x_key_state == KeyState::RELEASED && is_key_pressed(Key::X)) {
        set_key_pressed(Key::X, false);
    }
}

void App::main_loop(const char *json_filename) {
    using json = nlohmann::json;
    std::ifstream file(json_filename);
    json data = json::parse(file);
    double dt_multiplier = data["dt_multiplier"];

    std::string original_title = title();

    axolote::gl::Shader instanced_shader_program = axolote::gl::Shader(
        "./resources/shaders/instanced_vertex_shader.glsl",
        "./resources/shaders/fragment_shader.glsl"
    );

    // Celestial Body system
    bodies_system->setup_using_json(data);
    bodies_system->setup_instanced_vbo();
    bodies_system->bind_shader(instanced_shader_program);

    // Scene object
    auto scene = std::make_shared<axolote::Scene>();
    // Configs camera (points it downwards)
    scene->camera.fov = 70.0f;
    scene->camera.speed = 50.0f;
    scene->camera.sensitivity = 10000.0f;
    scene->camera.max_dist = 3000.0f;

    // Add system to scene
    scene->add_drawable(bodies_system);

    set_scene(scene);

    std::cout << "Press P to start/stop" << std::endl;
    pause = true;
    double before = get_time();
    while (!should_close()) {
        clear();

        poll_events();

        double now = get_time();
        double dt = now - before;
        before = now;
        process_input(dt);
        process_input_real_time_mode(dt);

        std::stringstream sstr;
        sstr << original_title << " | " << (int)(1 / dt) << " fps";
        set_title(sstr.str());

        if (!pause) {
            dt *= dt_multiplier;
            update(dt);
        }

        update_camera((float)width() / height());
        render();

        flush();
    }
}

void App::bake(const char *json_filename) {
    using json = nlohmann::json;
    std::ifstream file(json_filename);
    json data = json::parse(file);
    double dt_multiplier = data["dt_multiplier"];

    // Current scene is needed for process input from user
    auto scene = std::make_shared<axolote::Scene>();
    set_scene(scene);

    // Celestial Body system
    bodies_system->setup_using_json(data);

    std::cout << "LET HIM COOK!" << std::endl
              << "DO NOT PRESS Ctrl+C" << std::endl
              << "IF YOU WANT TO STOP PRESS ESC" << std::endl;

    std::string output_filename = std::string{json_filename} + ".baked";
    std::ofstream outputfile{output_filename};
    std::size_t counter = 0;
    outputfile << "[" << std::endl;
    while (!should_close()) {
        clear();

        poll_events();

        double dt = 1.0 / 60;
        dt *= dt_multiplier;

        process_input(dt);

        bodies_system->update(dt);

        int i = 0;
        int size = bodies_system->celestial_bodies().size();
        outputfile << "[";
        for (auto &c : bodies_system->celestial_bodies()) {
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

        flush();
    }

    outputfile << "]" << std::endl;
    std::cout << "Done!" << std::endl
              << "Content saved at: " << output_filename << std::endl;
}

void App::render_loop(const char *json_filename) {
    using json = nlohmann::json;

    std::string original_title = title();

    axolote::gl::Shader instanced_shader_program(
        "./resources/shaders/instanced_vertex_shader.glsl",
        "./resources/shaders/fragment_shader.glsl"
    );

    bool first_setup = true;
    bodies_system->bind_shader(instanced_shader_program);

    // Scene object
    auto scene = std::make_shared<axolote::Scene>();
    // Configs camera (points it downwards)
    scene->camera.fov = 70.0f;
    scene->camera.pos = glm::vec3{0.0f, 300.0f, 0.0f};
    scene->camera.orientation = glm::normalize(glm::vec3{0.01f, -1.0f, 0.0f});
    scene->camera.speed = 50.0f;
    scene->camera.sensitivity = 10000.0f;
    scene->camera.max_dist = 3000.0f;

    scene->add_drawable(bodies_system);

    set_scene(scene);

    std::ifstream file(json_filename);
    std::string line;
    file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    std::cout << "Press P to start/stop" << std::endl;
    pause = true;
    double before = get_time();
    while (!file.eof() && !should_close()) {
        clear();

        poll_events();

        double now = get_time();
        double dt = now - before;
        before = now;
        process_input(dt);

        std::stringstream sstr;
        sstr << original_title << " | " << (int)(1 / dt) << " fps";
        set_title(sstr.str());

        if (!pause) {
            std::getline(file, line);
            if (line == "]") {
                std::cout
                    << "Rendered all frames, press Ctrl+C, ESC or P to exit"
                    << std::endl;
                pause = true;
                continue;
            }
            else if (line.back() == ',') {
                line.pop_back();
            }

            json j = json::parse(line);

            bodies_system->setup_using_baked_frame_json(j);
            if (first_setup) {
                bodies_system->setup_instanced_vbo();
                first_setup = false;
            }
            else {
                bodies_system->update_vbos();
            }
        }

        update_camera((float)width() / height());
        render();

        flush();
    }
}
