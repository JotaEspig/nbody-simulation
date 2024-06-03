#pragma once

#include <memory>
#include <vector>

#include <axolote/engine.hpp>
#include <glm/glm.hpp>
#include <nlohmann/json.hpp>

#include "celestial_body.hpp"
#include "octree.hpp"
#include "sphere.hpp"

class CelestialBodySystem : public axolote::Drawable {
public:
    OcTree octree;
    Sphere sphere;

    CelestialBodySystem() = default;
    ~CelestialBodySystem() = default;

    void setup_using_json(nlohmann::json &data);
    void setup_using_baked_frame_json(nlohmann::json &data);
    void setup_instanced_vbo();
    std::shared_ptr<CelestialBody>
    add_body(double mass, glm::vec3 pos, glm::vec3 vel);
    void normal_algorithm(double dt);
    void barnes_hut_algorithm(double dt);
    std::vector<std::shared_ptr<CelestialBody>> celestial_bodies() const;

    void bind_shader(const axolote::gl::Shader &shader_program) override;
    axolote::gl::Shader get_shader() const override;
    void update(double dt) override;
    void draw() override;
    void draw(const glm::mat4 &mat) override;

private:
    axolote::gl::VBO instanced_matrices_vbo;
    std::vector<std::shared_ptr<CelestialBody>> _celestial_bodies;

    void build_octree();
};
