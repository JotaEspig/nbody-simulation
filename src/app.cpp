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
#include <glm/gtx/norm.hpp>
#include <glm/gtx/string_cast.hpp>

#include "app.hpp"
#include "gravitational_grid.hpp"
#include "utils.hpp"

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

void App::process_input() {
    KeyState l_key_state = get_key_state(Key::L);
    if (l_key_state == KeyState::PRESSED && !is_key_pressed(Key::L)) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        set_key_pressed(Key::L, true);
    }
    else if (l_key_state == KeyState::RELEASED && is_key_pressed(Key::L)) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        set_key_pressed(Key::L, false);
    }

    KeyState pause_key_state = get_key_state(Key::P);
    if (pause_key_state == KeyState::PRESSED && !is_key_pressed(Key::P)) {
        current_scene()->pause = !current_scene()->pause;
        set_key_pressed(Key::P, true);
    }
    else if (pause_key_state == KeyState::RELEASED && is_key_pressed(Key::P)) {
        set_key_pressed(Key::P, false);
    }

    if (get_key_state(Key::ESCAPE) == KeyState::PRESSED) {
        set_should_close(true);
    }
    if (get_key_state(Key::R) == KeyState::PRESSED) {
        focus_point = glm::vec3{0.0f, 0.0f, 0.0f};
        distance = 300.0f;
    }

    update_focus_point();
    update_camera_position();
}

void App::process_input_real_time_mode() {
    KeyState x_key_state = get_key_state(Key::X);
    if (x_key_state == KeyState::PRESSED && !is_key_pressed(Key::X)) {
        glm::vec3 pos = current_scene()->context->camera.get_pos();
        glm::vec3 vel = (1.0f / 40000)
                        * current_scene()->context->camera.get_orientation();

        bodies_system->add_body(100, pos, vel);

        set_key_pressed(Key::X, true);
    }
    else if (x_key_state == KeyState::RELEASED && is_key_pressed(Key::X)) {
        set_key_pressed(Key::X, false);
    }
}

void App::main_loop(const char *json_filename, bool use_grav_grid) {
    set_color(0.0f, 0.0f, 0.0f, 1.0f);
    using json = nlohmann::json;
    std::ifstream file(json_filename);
    json data = json::parse(file);
    double dt_multiplier = data["dt_multiplier"];

    std::string original_title = title();

    auto instanced_shader_program = axolote::gl::Shader::create(
        path("resources/shaders/instanced_vertex_shader.glsl"),
        path("resources/shaders/fragment_shader.glsl")
    );
    auto post_processing_shader = axolote::gl::Shader::create(
        path("resources/shaders/post_processing_base_vertex_shader.glsl"),
        path("resources/shaders/post_processing_base_fragment_shader.glsl")
    );
    auto gmesh_shader = axolote::gl::Shader::create(
        path("resources/shaders/gravgrid_vertex_shader.glsl"),
        path("resources/shaders/gmesh_base_fragment_shader.glsl")
    );

    // Celestial Body system
    bodies_system->setup_using_json(data);
    bodies_system->setup_instanced_vbo();
    bodies_system->bind_shader(instanced_shader_program);

    // Scene object
    auto scene = std::make_shared<axolote::Scene>();
    scene->renderer.init(width(), height());
    scene->renderer.setup_shader(post_processing_shader);
    // Configs camera (points it downwards)
    scene->context->camera.fov = 70.0f;
    scene->context->camera.speed = 50.0f;
    scene->context->camera.max_dist = 3000.0f;
    latitude = 30.0f;

    if (use_grav_grid) {
        auto bodies = bodies_system->celestial_bodies();
        std::sort(
            bodies.begin(), bodies.end(),
            [](const std::shared_ptr<CelestialBody> &a,
               const std::shared_ptr<CelestialBody> &b) {
                if (a->pos.x > b->pos.x) {
                    return true;
                }
                else if (a->pos.x < b->pos.x) {
                    return false;
                }
                else if (a->pos.z > b->pos.z) {
                    return true;
                }
                else {
                    return false;
                }
            }
        );
        glm::vec3 biggest = bodies[0]->pos;
        int amount = (int)glm::length(biggest) + 30;
        std::cout << amount << std::endl;
        std::cout << amount / 30 << std::endl;
        auto grav_grid = std::make_shared<GravGrid>(
            bodies_system, amount, amount / 30
        );
        grav_grid->bind_shader(gmesh_shader);
        scene->add_drawable(grav_grid);
    }

    scene->add_drawable(bodies_system);

    set_scene(scene);

    std::cout << "Press P to start/stop" << std::endl;
    pause = true;
    while (!should_close()) {
        init_frame();

        process_input();
        process_input_real_time_mode();

        std::stringstream sstr;
        sstr << original_title << " | " << (int)(1 / _delta_time) << " fps";
        set_title(sstr.str());

        update_camera((float)width() / height());
        _delta_time *= dt_multiplier;
        update();
        render();

        finish_frame();
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

        process_input();

        bodies_system->update(_absolute_time, dt);

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

        finish_frame();
    }

    outputfile << "]" << std::endl;
    std::cout << "Done!" << std::endl
              << "Content saved at: " << output_filename << std::endl;
}

void App::render_loop(const char *json_filename) {
    using json = nlohmann::json;

    std::string original_title = title();

    auto instanced_shader_program = axolote::gl::Shader::create(
        "./resources/shaders/instanced_vertex_shader.glsl",
        "./resources/shaders/fragment_shader.glsl"
    );

    bool first_setup = true;
    bodies_system->bind_shader(instanced_shader_program);

    // Scene object
    auto scene = std::make_shared<axolote::Scene>();
    // Configs camera (points it downwards)
    scene->context->camera.fov = 70.0f;
    scene->context->camera.set_pos(glm::vec3{0.0f, 300.0f, 0.0f});
    scene->context->camera.set_orientation(
        glm::normalize(glm::vec3{0.01f, -1.0f, 0.0f})
    );
    scene->context->camera.speed = 50.0f;
    scene->context->camera.sensitivity = 10000.0f;
    scene->context->camera.max_dist = 3000.0f;

    scene->add_drawable(bodies_system);

    set_scene(scene);

    std::ifstream file(json_filename);
    std::string line;
    file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    std::cout << "Press P to start/stop" << std::endl;
    pause = true;
    while (!file.eof() && !should_close()) {
        poll_events();
        tick();

        process_input();

        std::stringstream sstr;
        sstr << original_title << " | " << (int)(1 / _delta_time) << " fps";
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

        finish_frame();
    }
}

void App::update_focus_point() {
    // Move focus point using arrow keys and right control and right shift
    // according to camera orientation
    float speed = current_scene()->context->camera.speed / 2.0f;
    glm::vec3 dir = current_scene()->context->camera.get_orientation();
    if (get_key_state(Key::UP) == KeyState::PRESSED) {
        focus_point += dir * (float)_delta_time * speed;
    }
    if (get_key_state(Key::DOWN) == KeyState::PRESSED) {
        focus_point -= dir * (float)_delta_time * speed;
    }
    if (get_key_state(Key::LEFT) == KeyState::PRESSED) {
        focus_point
            -= glm::normalize(glm::cross(dir, glm::vec3{0.0f, 1.0f, 0.0f}))
               * (float)_delta_time * speed;
    }
    if (get_key_state(Key::RIGHT) == KeyState::PRESSED) {
        focus_point
            += glm::normalize(glm::cross(dir, glm::vec3{0.0f, 1.0f, 0.0f}))
               * (float)_delta_time * speed;
    }
    if (get_key_state(Key::RIGHT_SHIFT) == KeyState::PRESSED) {
        focus_point += current_scene()->context->camera.get_up()
                       * (float)_delta_time * speed;
    }
    if (get_key_state(Key::RIGHT_CONTROL) == KeyState::PRESSED) {
        focus_point -= current_scene()->context->camera.get_up()
                       * (float)_delta_time * speed;
    }
}

void App::update_camera_position() {
    /** Constant multiplied when distance is modified my camera movement,
     * that's because it seems slower than latitude and longitude movements **/
    float distance_modifier = 2.0f;
    if (get_key_state(Key::W) == KeyState::PRESSED) {
        distance -= distance_modifier * current_scene()->context->camera.speed
                    * _delta_time;
    }
    if (get_key_state(Key::A) == KeyState::PRESSED) {
        longitude += current_scene()->context->camera.speed * _delta_time;
    }
    if (get_key_state(Key::S) == KeyState::PRESSED) {
        distance += distance_modifier * current_scene()->context->camera.speed
                    * _delta_time;
    }
    if (get_key_state(Key::D) == KeyState::PRESSED) {
        longitude -= current_scene()->context->camera.speed * _delta_time;
    }
    if (get_key_state(Key::SPACE) == KeyState::PRESSED) {
        latitude += current_scene()->context->camera.speed * _delta_time;
    }
    if (get_key_state(Key::LEFT_SHIFT) == KeyState::PRESSED) {
        latitude -= current_scene()->context->camera.speed * _delta_time;
    }

    distance = glm::max(distance, 10.0f);
    latitude = glm::clamp(latitude, -89.0f, 89.0f);

    // Update camera position according to latitude, longitude and distance and
    // camera must be looking at focus point
    glm::vec3 pos;
    pos.x
        = distance * cos(glm::radians(latitude)) * cos(glm::radians(longitude));
    pos.y = distance * sin(glm::radians(latitude));
    pos.z
        = distance * cos(glm::radians(latitude)) * sin(glm::radians(longitude));
    pos += focus_point;
    current_scene()->context->camera.set_pos(pos);
    current_scene()->context->camera.set_orientation(
        glm::normalize(focus_point - current_scene()->context->camera.get_pos())
    );
}
