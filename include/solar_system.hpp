#pragma once

#include <memory>
#include <vector>

#include <axolote/engine.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <celestial_body.hpp>

class SolarSystem
{
public:
    SolarSystem() = default;
    ~SolarSystem() = default;

    std::shared_ptr<CelestialBody> add_celestial_body(
        double mass, glm::vec3 pos, glm::vec3 vel, glm::vec3 color,
        axolote::gl::Shader shader_program
    );
    void update(double dt);

private:
    std::vector<std::shared_ptr<CelestialBody>> celestial_bodies;
};
