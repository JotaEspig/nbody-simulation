#pragma once

#include <memory>
#include <vector>

#include <axolote/engine.hpp>
#include <glm/glm.hpp>

#include "celestial_body.hpp"
#include "octree.hpp"

class CelestialBodySystem
{
public:
    OcTree octree;
    std::shared_ptr<axolote::Model> default_body_model{new axolote::Model{
        "./resources/models/sphere/sphere.obj", glm::vec3{1.0f, 1.0f, 1.0f}
    }};

    CelestialBodySystem() = default;
    ~CelestialBodySystem() = default;

    void
    setup_using_json(axolote::gl::Shader shader_program, const char *filename);
    std::shared_ptr<CelestialBody> add_celestial_body(
        double mass, glm::vec3 pos, glm::vec3 vel,
        axolote::gl::Shader shader_program
    );
    void build_octree();
    void update(double dt);
    std::vector<std::shared_ptr<CelestialBody>> celestial_bodies() const;

private:
    std::vector<std::shared_ptr<CelestialBody>> _celestial_bodies;
};
