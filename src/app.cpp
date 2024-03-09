#include <memory>
#include <sstream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#include <app.hpp>
#include <celestial_body.hpp>
#include <celestial_body_system.hpp>

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
        "./resources/shaders/def_vertex_shader.glsl",
        "./resources/shaders/def_fragment_shader.glsl"
    );

    shader_program.activate();
    shader_program.set_uniform_int("light.is_set", 0);

    // --------------------------------------------------------------------

    // Celestial Body system
    CelestialBodySystem ss;
    ss.setup_using_json(shader_program, json_filename);

    // Scene object
    std::shared_ptr<axolote::Scene> scene{new axolote::Scene{}};
    scene->camera.pos = glm::vec3{0.0f, 200.0f, 0.0f};
    scene->camera.orientation = glm::normalize(glm::vec3{.1f, -1.0f, 0.0f});
    scene->camera.speed = 80.0f;
    scene->camera.sensitivity = 10000.0f;

    // Add celestial bodies as drawable to scene
    for (auto &e : ss.celestial_bodies())
    {
        scene->add_drawable(e);
    }

    current_scene = scene;
    // auto x = ss.celestial_bodies();
    // std::shared_ptr<CelestialBody> black_hole;
    // for (auto &e : x)
    //{
    //     if (e->is_black_hole == true)
    //     {
    //         current_scene->camera.orientation
    //             = glm::normalize(e->pos - current_scene->camera.pos);
    //         black_hole = e;
    //         break;
    //     }
    // }

    double before = glfwGetTime();
    while (!should_close())
    {
        glClearColor(_color.r, _color.g, _color.b, _color.opacity);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glfwPollEvents();

        //    current_scene->camera.orientation
        //        = glm::normalize(black_hole->pos - current_scene->camera.pos);

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
