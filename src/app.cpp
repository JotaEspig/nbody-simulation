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
    scene->camera.orientation = glm::normalize(glm::vec3{0.01f, -1.0f, 0.0f});
    scene->camera.speed = 80.0f;
    scene->camera.sensitivity = 10000.0f;

    // Add celestial bodies as drawable to scene
    for (auto &e : ss.celestial_bodies())
    {
        scene->add_drawable(e);
    }

    current_scene = scene;
    double before = glfwGetTime();
    while (!should_close())
    {
        glClearColor(_color.r, _color.g, _color.b, _color.opacity);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glfwPollEvents();

        // auto x = ss.celestial_bodies();
        // glm::vec3 mid_point{
        //     (x[0]->pos.x + x[1]->pos.x + x[2]->pos.x) / 3,
        //     (x[0]->pos.y + x[1]->pos.y + x[2]->pos.y) / 3,
        //     (x[0]->pos.z + x[1]->pos.z + x[2]->pos.z) / 3
        // };
        // current_scene->camera.orientation
        //     = glm::normalize(mid_point - current_scene->camera.pos);

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

            ss.build_octree();
            ss.update(dt);

            scene->update(dt);
        }

        scene->update_camera((float)width() / height());
        scene->render();

        glfwSwapBuffers(window);
    }
}
