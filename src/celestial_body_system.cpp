#include <memory>

#include <axolote/object3d.hpp>

#include <celestial_body_system.hpp>

std::shared_ptr<CelestialBody> CelestialBodySystem::add_celestial_body(
    double mass, glm::vec3 pos, glm::vec3 vel,
    axolote::gl::Shader shader_program
)
{
    // Create object matrix
    glm::mat4 mat{1.0f};
    mat = glm::translate(mat, pos);

    // Create body
    std::shared_ptr<CelestialBody> body{new CelestialBody{mass, vel, pos}};
    axolote::Object3D obj{mat};
    obj.model = default_body_model;
    body->add_object(obj);
    body->bind_shader_at(0, shader_program);

    // Add to list
    celestial_bodies.push_back(body);

    return body;
}

void CelestialBodySystem::update(double dt)
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
