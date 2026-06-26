#define GLM_ENABLE_EXPERIMENTAL
#define DEBUG

#include <algorithm>
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

#include <axolote/engine.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <nlohmann/json.hpp>

#include "app.hpp"
#include "gravitational_grid.hpp"
#include "utils.hpp"

#define UNUSED(x) (void)(x)

struct BenchmarkResult {
    CelestialBodySystem::SimulationAlgorithm algorithm;
    std::string name;

    std::size_t bodies;
    std::size_t iterations;
    double elapsed;
    double steps_per_second;
    double bodies_steps_per_second;
    double simulated_seconds_per_second;
};

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
    glfwSetWindowUserPointer(window(), this);
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
    auto gravgrid_shader = axolote::gl::Shader::create(
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
        auto grav_grid = std::make_shared<GravGrid>(bodies);
        grav_grid->bind_shader(gravgrid_shader);
        scene->add_drawable(grav_grid);

        bodies_system->grav_grid = grav_grid;
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

void App::benchmark(const char *json_filename, std::chrono::seconds duration) {
    using json = nlohmann::json;

    std::ifstream file(json_filename);
    json data = json::parse(file);

    const double dt = (1.0 / 60.0) * static_cast<double>(data["dt_multiplier"]);

    const std::vector<BenchmarkEntry> algorithms
        = {{CelestialBodySystem::SimulationAlgorithm::Naive, "Naive O(n²)"},
           {CelestialBodySystem::SimulationAlgorithm::BarnesHut, "Barnes-Hut"},
           {CelestialBodySystem::SimulationAlgorithm::BarnesHutOpenMP,
            "Barnes-Hut + OpenMP"}};

    std::cout << "=============================================\n";
    std::cout << "Benchmark\n";
    std::cout << "Configuration : " << json_filename << '\n';
    std::cout << "Duration      : " << duration.count() << " s each\n";
    std::cout << "=============================================\n\n";

    std::vector<BenchmarkResult> results;

    for (const auto &benchmark : algorithms) {
        // Reset simulation
        bodies_system->setup_using_json(data);
        bodies_system->algorithm = benchmark.algorithm;

        // Warm-up (not measured)
        std::cout << "Warming up...\n";
        constexpr int warmup_iterations = 10;
        for (int i = 0; i < warmup_iterations; ++i) {
            bodies_system->simulate(dt);
        }
        std::cout << "Finished warm up\n";

        // Restart simulation so every algorithm starts from the same state
        bodies_system->setup_using_json(data);

        auto start = std::chrono::steady_clock::now();
        auto end = start + duration;

        std::size_t iterations = 0;

        while (std::chrono::steady_clock::now() < end) {
            bodies_system->simulate(dt);
            ++iterations;
        }

        auto elapsed = std::chrono::duration<double>(
            std::chrono::steady_clock::now() - start
        );

        const auto body_count = bodies_system->celestial_bodies().size();

        std::cout << benchmark.name << '\n';
        std::cout << "---------------------------------------------\n";
        std::cout << "Bodies                : " << body_count << '\n';
        std::cout << "Elapsed               : " << elapsed.count() << " s\n";
        std::cout << "Simulation steps      : " << iterations << '\n';
        std::cout << "Steps / second        : " << iterations / elapsed.count()
                  << '\n';
        std::cout << "Bodies·steps / second : "
                  << (iterations * body_count) / elapsed.count() << '\n';
        std::cout << "Simulated seconds/sec : "
                  << (iterations * dt) / elapsed.count() << '\n';
        std::cout << '\n';
        results.push_back(
            {benchmark.algorithm, benchmark.name, body_count, iterations,
             elapsed.count(), iterations / elapsed.count(),
             (iterations * body_count) / elapsed.count(),
             (iterations * dt) / elapsed.count()}
        );
    }

    std::cout << "=============================================\n";
    std::cout << "Benchmark Summary\n";
    std::cout << "=============================================\n\n";

    std::cout << std::left << std::setw(24) << "Algorithm" << std::right
              << std::setw(15) << "Steps/s" << std::setw(15) << "Speedup"
              << std::setw(18) << "Sim. sec/s" << '\n';

    std::cout << std::string(72, '-') << '\n';

    const double baseline = results.front().steps_per_second;

    for (const auto &r : results) {
        std::cout << std::left << std::setw(24) << r.name << std::right
                  << std::setw(15) << std::fixed << std::setprecision(2)
                  << r.steps_per_second << std::setw(14)
                  << r.steps_per_second / baseline << "x" << std::setw(18)
                  << r.simulated_seconds_per_second << '\n';
    }

    std::cout << '\n';

    const auto &naive = results[0];
    const auto &barnes = results[1];
    const auto &omp = results[2];

    auto printComparison = [](const BenchmarkResult &a,
                              const BenchmarkResult &b) {
        const double speedup = b.steps_per_second / a.steps_per_second;
        const double improvement = (b.steps_per_second - a.steps_per_second)
                                   / a.steps_per_second * 100.0;

        std::cout << b.name << " vs " << a.name << '\n'
                  << "  Speedup     : " << std::fixed << std::setprecision(2)
                  << speedup << "x\n"
                  << "  Improvement : +" << improvement << "%\n\n";
    };

    printComparison(naive, barnes);
    printComparison(barnes, omp);
    printComparison(naive, omp);

    const auto winner = std::max_element(
        results.begin(), results.end(),
        [](const BenchmarkResult &a, const BenchmarkResult &b) {
            return a.steps_per_second < b.steps_per_second;
        }
    );

    std::cout << "Winner: " << winner->name << '\n';
    std::cout << "=============================================\n";
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
