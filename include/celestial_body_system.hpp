#pragma once

#include <memory>
#include <vector>

#include <axolote/engine.hpp>
#include <glm/glm.hpp>

#include <celestial_body.hpp>

class CelestialBodySystem
{
public:
    std::shared_ptr<axolote::Model> default_body_model{new axolote::Model{
        "./resources/models/sphere/sphere.obj", glm::vec3{1.0f, 1.0f, 1.0f}
    }};

    CelestialBodySystem() = default;
    ~CelestialBodySystem() = default;

    std::shared_ptr<CelestialBody> add_celestial_body(
        double mass, glm::vec3 pos, glm::vec3 vel,
        axolote::gl::Shader shader_program
    );
    void update(double dt);

private:
    std::vector<std::shared_ptr<CelestialBody>> celestial_bodies;
};
