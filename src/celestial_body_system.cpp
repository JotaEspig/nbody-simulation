#include <iostream>
#include <memory>

#include <axolote/object3d.hpp>
#include <nlohmann/json.hpp>

#include "celestial_body_system.hpp"
#include "octree.hpp"

void CelestialBodySystem::setup_using_json(
    axolote::gl::Shader shader_program, nlohmann::json &data
) {
    assert(std::strlen(filename) > 0);

    using json = nlohmann::json;

    json bodies = data["bodies"];
    for (auto &e : bodies) {
        glm::vec3 pos;
        glm::vec3 vel;
        pos.x = e["pos"]["x"];
        pos.y = e["pos"]["y"];
        pos.z = e["pos"]["z"];
        vel.x = e["velocity"]["x"];
        vel.y = e["velocity"]["y"];
        vel.z = e["velocity"]["z"];
        add_celestial_body(e["mass"], pos, vel, shader_program);
    }
}

std::shared_ptr<CelestialBody> CelestialBodySystem::add_celestial_body(
    double mass, glm::vec3 pos, glm::vec3 vel,
    axolote::gl::Shader shader_program
) {
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
    _celestial_bodies.push_back(body);

    return body;
}

void CelestialBodySystem::build_octree() {
    octree = OcTree{};
    for (auto &c : celestial_bodies()) {
        octree.insert(c);
    }
}

void CelestialBodySystem::normal_algorithm(double dt) {
    for (auto body0 : _celestial_bodies) {
        for (auto body1 : _celestial_bodies) {
            if (body0 == body1)
                continue;

            glm::vec3 acc = body0->calculate_acceleration_vec(*body1);
            body0->velocity += acc * (float)dt;
        }
    }
}

void CelestialBodySystem::barnes_hut_algorithm(double dt) {
    build_octree();

    std::vector<std::shared_ptr<CelestialBody>> active_bodies;
    for (auto &c : _celestial_bodies) {
        bool should_erase = std::abs(c->pos.x) > octree.initial_width / 2
                            || std::abs(c->pos.y) > octree.initial_width / 2
                            || std::abs(c->pos.z) > octree.initial_width / 2
                            || c->merged;
        if (!should_erase) {
            active_bodies.push_back(c);
            glm::vec3 acc = octree.net_acceleration_on_body(c, dt);
            c->velocity += acc * (float)dt;
        }
    }

    _celestial_bodies = std::move(active_bodies);
}

void CelestialBodySystem::update(double dt) {
    // normal_algorithm(dt);
    barnes_hut_algorithm(dt);
}

std::vector<std::shared_ptr<CelestialBody>>
CelestialBodySystem::celestial_bodies() const {
    return _celestial_bodies;
}
