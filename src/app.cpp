#include <GLFW/glfw3.h>
#include <memory>
#include <sstream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#include <app.hpp>
#include <celestial_body.hpp>
#include <solar_system.hpp>

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

void App::main_loop()
{
    std::string original_title = title();

    axolote::gl::Shader shader_program(
        "./resources/shaders/def_vertex_shader.glsl",
        "./resources/shaders/def_fragment_shader.glsl"
    );

    shader_program.activate();
    shader_program.set_uniform_float("ambient", 0.05f);
    shader_program.set_uniform_float4("light_color", 1.0f, 1.0f, 1.0f, 1.0f);
    shader_program.set_uniform_float3("light_pos", 0.0f, 0.0f, 0.0f);
    // Hardcoded Sun object radius
    shader_program.set_uniform_float("light_radius", 5.0f);

    // Table with planets data:
    // https://nssdc.gsfc.nasa.gov/planetary/factsheet/

    // website used for initial velocity calculation:
    // https://pt.calcprofi.com/calculadora-formula-velocidade-orbital.html
    // using: parâmetros -> km, kg and m/s

    // --------------------------------------------------------------------

    // Solar system
    SolarSystem ss;

    // Sun
    std::shared_ptr<CelestialBody> sun = ss.add_celestial_body(
        333000.0,                         // mass (related to earth)
        glm::vec3{0.0f, 0.0f, 0.0f},      // pos
        glm::vec3{0.0, -0.000005f, 0.0f}, // vel
        glm::vec3{1.0f, 1.0f, 0.5f},      // color
        shader_program                    // shader
    );
    sun->is_light_emissor = true;

    // Mercury
    std::shared_ptr<CelestialBody> mercury = ss.add_celestial_body(
        0.055,                            // mass (related to earth)
        glm::vec3{0.0f, 0.0f, 60.0f},     // pos
        glm::vec3{0.000609f, 0.0f, 0.0f}, // vel
        glm::vec3{0.28f, 0.27f, 0.27f},   // color
        shader_program                    // shader
    );

    // Venus
    std::shared_ptr<CelestialBody> venus = ss.add_celestial_body(
        0.81,                             // mass (related to earth)
        glm::vec3{0.0f, 0.0f, 75.0f},     // pos
        glm::vec3{0.000544f, 0.0f, 0.0f}, // vel
        glm::vec3{0.35f, 0.51f, 0.89f},   // color
        shader_program                    // shader
    );

    // Earth
    std::shared_ptr<CelestialBody> earth = ss.add_celestial_body(
        1.0,                              // mass (related to earth)
        glm::vec3{0.0f, 0.0f, 90.0f},     // pos
        glm::vec3{0.000497f, 0.0f, 0.0f}, // vel
        glm::vec3{0.0f, 0.0f, 1.0f},      // color
        shader_program                    // shader
    );

    // Mars
    std::shared_ptr<CelestialBody> mars = ss.add_celestial_body(
        0.11,                            // mass (related to earth)
        glm::vec3{0.0f, 0.0f, 105.0f},   // pos
        glm::vec3{0.00046f, 0.0f, 0.0f}, // vel
        glm::vec3{0.75f, 0.25f, 0.05f},  // color
        shader_program                   // shader
    );

    // Jupiter
    std::shared_ptr<CelestialBody> jupiter = ss.add_celestial_body(
        317.82,                           // mass (related to earth)
        glm::vec3{0.0f, 0.0f, 150.0f},    // pos
        glm::vec3{0.000385f, 0.0f, 0.0f}, // vel
        glm::vec3{0.78f, 0.56f, 0.22f},   // color
        shader_program                    // shader
    );

    // Saturn
    std::shared_ptr<CelestialBody> saturn = ss.add_celestial_body(
        95.2,                             // mass (related to earth)
        glm::vec3{0.0f, 0.0f, 180.0f},    // pos
        glm::vec3{0.000351f, 0.0f, 0.0f}, // vel
        glm::vec3{0.07f, 0.14f, 0.32f},   // color
        shader_program                    // shader
    );

    // Uranus
    std::shared_ptr<CelestialBody> uranus = ss.add_celestial_body(
        14.5,                             // mass (related to earth)
        glm::vec3{0.0f, 0.0f, 205.0f},    // pos
        glm::vec3{0.000329f, 0.0f, 0.0f}, // vel
        glm::vec3{0.11f, 0.06f, 0.06f},   // color
        shader_program                    // shader
    );

    // Neptune
    std::shared_ptr<CelestialBody> neptune = ss.add_celestial_body(
        17.1,                             // mass (related to earth)
        glm::vec3{0.0f, 0.0f, 230.0f},    // pos
        glm::vec3{0.000311f, 0.0f, 0.0f}, // vel
        glm::vec3{0.64f, 0.63f, 0.12f},   // color
        shader_program                    // shader
    );

    // Pluto
    std::shared_ptr<CelestialBody> pluto = ss.add_celestial_body(
        0.0022,                           // mass (related to earth)
        glm::vec3{0.0f, 0.0f, 260.0f},    // pos
        glm::vec3{0.000292f, 0.0f, 0.0f}, // vel
        glm::vec3{0.0f, 0.05f, 0.16f},    // color
        shader_program                    // shader
    );

    // Scene object
    std::shared_ptr<axolote::Scene> scene{new axolote::Scene{}};
    scene->camera.pos = glm::vec3{0.0f, 0.0f, 0.0f};
    scene->camera.orientation = glm::normalize(glm::vec3{.1f, -1.0f, 0.0f});
    scene->camera.speed = 3.0f;
    scene->camera.sensitivity = 10000.0f;

    // Add celestial bodies as drawable to scene
    scene->add_drawable(sun);
    scene->add_drawable(mercury);
    scene->add_drawable(venus);
    scene->add_drawable(earth);
    scene->add_drawable(mars);
    scene->add_drawable(jupiter);
    scene->add_drawable(saturn);
    scene->add_drawable(uranus);
    scene->add_drawable(neptune);
    scene->add_drawable(pluto);

    current_scene = scene;
    double before = glfwGetTime();
    while (!should_close())
    {
        shader_program.set_uniform_float3(
            "light_pos", sun->pos.x, sun->pos.y, sun->pos.z
        );

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
