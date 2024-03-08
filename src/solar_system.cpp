#include <memory>
#include <solar_system.hpp>

#include <axolote/object3d.hpp>

std::shared_ptr<CelestialBody> SolarSystem::add_celestial_body(
    double mass, glm::vec3 pos, glm::vec3 vel, glm::vec3 color,
    axolote::gl::Shader shader_program
)
{
    // Create object matrix
    glm::mat4 mat{1.0f};
    mat = glm::translate(mat, pos);

    // Create body
    std::shared_ptr<CelestialBody> body{new CelestialBody{mass, vel}};
    auto bodyobj = std::make_shared<axolote::Object3D>(
        "./resources/models/sphere/sphere.obj", color, mat
    );
    body->pos = pos;
    body->add_object(bodyobj);
    body->bind_shader_at(0, shader_program);

    // Add to list
    celestial_bodies.push_back(body);

    return body;
}

void SolarSystem::update(double dt)
{
    for (auto body0 : celestial_bodies)
    {
        for (auto body1 : celestial_bodies)
        {
            if (body0 == body1)
                continue;

            glm::vec3 acc = body0->calculate_acceleration_vec(*body1);
            body1->velocity += acc * (float)dt;
        }
    }
}
