#include <iostream>
#include <memory>
#include <sstream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#include "app.hpp"
#include "celestial_body_system.hpp"
#include "octree.hpp"

void App::process_input(float delta_t)
{
    Window::process_input(delta_t);

    int pause_key_state = glfwGetKey(window, GLFW_KEY_P);
    if (pause_key_state == GLFW_PRESS && !is_pause_key_press)
    {
        is_pause_key_press = true;
    }
    else if (pause_key_state == GLFW_RELEASE && is_pause_key_press)
    {
        pause = !pause;
        is_pause_key_press = false;
    }
}

void App::main_loop(const char *json_filename)
{
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
    ss.setup_using_json(shader_program, json_filename);

    // Scene object
    std::shared_ptr<axolote::Scene> scene{new axolote::Scene{}};
    // Configs camera (points it downwards)
    scene->camera.pos = glm::vec3{0.0f, 100.0f, 0.0f};
    scene->camera.orientation = glm::normalize(glm::vec3{.1f, -1.0f, 0.0f});
    scene->camera.speed = 80.0f;
    scene->camera.sensitivity = 10000.0f;

    // Add celestial bodies as drawable to scene
    for (auto &e : ss.celestial_bodies())
    {
        scene->add_drawable(e);
    }

    // Testing OcTree
    auto octree = OcTree(-1000.0f);
    auto body = std::make_shared<CelestialBody>(
        1, glm::vec3{}, glm::vec3{10.0f, 10.0f, 40.0f}
    );
    auto body2 = std::make_shared<CelestialBody>(
        1, glm::vec3{}, glm::vec3{-10.0f, 10.0f, 40.0f}
    );
    auto body3 = std::make_shared<CelestialBody>(
        1, glm::vec3{}, glm::vec3{10.0f, -100.0f, -40.0f}
    );
    auto body4 = std::make_shared<CelestialBody>(
        1, glm::vec3{}, glm::vec3{10.0f, 600.0f, 40.0f}
    );
    auto body5 = std::make_shared<CelestialBody>(
        1, glm::vec3{}, glm::vec3{10.0f, 600.0f, 700.0f}
    );

    std::cout << 1 << std::endl;
    octree.insert(body);
    std::cout << 2 << std::endl;
    octree.insert(body2);
    std::cout << 3 << std::endl;
    octree.insert(body3);
    std::cout << 4 << std::endl;
    octree.insert(body4);
    std::cout << 5 << std::endl;
    octree.insert(body5);

    std::cout << "aaa" << std::endl;
    std::cout << glm::to_string(octree.root->center()) << std::endl;
    std::cout << octree.root << std::endl;

    std::cout << "bbb" << std::endl;
    std::cout << glm::to_string(octree.root->luf->center()) << std::endl;
    std::cout << octree.root->luf << std::endl;
    std::cout << glm::to_string(octree.root->ruf->center()) << std::endl;
    std::cout << octree.root->ruf << std::endl;
    std::cout << glm::to_string(octree.root->rbb->center()) << std::endl;
    std::cout << octree.root->rbb << std::endl;

    std::cout << "ccc" << std::endl;

    current_scene = scene;
    double before = glfwGetTime();
    while (!should_close())
    {
        glClearColor(_color.r, _color.g, _color.b, _color.opacity);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glfwPollEvents();

        auto x = ss.celestial_bodies();
        glm::vec3 mid_point{
            (x[0]->pos.x + x[1]->pos.x) / 2, (x[0]->pos.y + x[1]->pos.y) / 2,
            (x[0]->pos.z + x[1]->pos.z) / 2
        };
        current_scene->camera.orientation
            = glm::normalize(mid_point - current_scene->camera.pos);

        double now = glfwGetTime();
        double dt = now - before;
        before = now;
        process_input(dt);

        std::stringstream sstr;
        sstr << original_title << " | " << (int)(1 / dt) << " fps";
        set_title(sstr.str());

        if (!pause)
        {
            dt *= DT_MULTIPLIER;

            // Update celestial bodies
            for (int i = 0; i < 10; i++)
                ss.update(dt / 10.0);

            scene->update(dt);
        }

        scene->update_camera((float)width() / height());
        scene->render();

        glfwSwapBuffers(window);
    }
}
