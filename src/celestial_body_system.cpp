#include <cstring>
#include <fstream>
#include <iostream>
#include <memory>

#include <axolote/object3d.hpp>
#include <nlohmann/json.hpp>

#include <celestial_body.hpp>
#include <celestial_body_system.hpp>

void CelestialBodySystem::setup_using_json(
    axolote::gl::Shader shader_program, const char *filename
)
{
    assert(std::strlen(filename) > 0);

    using json = nlohmann::json;

    std::ifstream file(filename);
    json data = json::parse(file);
    std::cout << "JSON read:" << std::endl;
    for (auto &e : data)
    {
        std::cout << e.dump() << std::endl;

        glm::vec3 pos;
        glm::vec3 vel;
        pos.x = e["pos"]["x"];
        pos.y = e["pos"]["y"];
        pos.z = e["pos"]["z"];
        vel.x = e["velocity"]["x"];
        vel.y = e["velocity"]["y"];
        vel.z = e["velocity"]["z"];

        bool is_black_hole = e["black_hole"];
        add_celestial_body(e["mass"], pos, vel, is_black_hole, shader_program);
    }
}

std::shared_ptr<CelestialBody> CelestialBodySystem::add_celestial_body(
    double mass, glm::vec3 pos, glm::vec3 vel, bool is_black_hole,
    axolote::gl::Shader shader_program
)
{
    // Create object matrix
    glm::mat4 mat{1.0f};
    mat = glm::translate(mat, pos);

    // Create body
    std::shared_ptr<CelestialBody> body;
    body = std::make_shared<CelestialBody>(mass, vel, pos, is_black_hole);

    axolote::Object3D obj{mat};
    if (is_black_hole)
        obj.model = default_black_hole_body_model;
    else
        obj.model = default_body_model;
    body->add_object(obj);
    body->bind_shader_at(0, shader_program);

    // Add to list
    _celestial_bodies.push_back(body);

    return body;
}

void CelestialBodySystem::update(double dt)
{
    for (auto body0 : _celestial_bodies)
    {
        for (auto body1 : _celestial_bodies)
        {
            if (body0 == body1)
                continue;

            glm::vec3 acc = body0->calculate_acceleration_vec(*body1);
            body1->velocity += acc * (float)dt;
        }
    }
}

std::vector<std::shared_ptr<CelestialBody>>
CelestialBodySystem::celestial_bodies() const
{
    return _celestial_bodies;
}
