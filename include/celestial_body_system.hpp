#pragma once

#include <memory>
#include <vector>

#include <axolote/engine.hpp>
#include <glm/glm.hpp>
#include <nlohmann/json.hpp>

#include "celestial_body.hpp"
#include "octree.hpp"

class CelestialBodySystem : public axolote::Drawable {
public:
    OcTree octree;
    std::shared_ptr<axolote::GModel> gmodel{new axolote::GModel{
        "./resources/models/sphere/sphere.obj", glm::vec3{0.0f, 0.3f, 1.0f}
    }};

    CelestialBodySystem() = default;
    ~CelestialBodySystem() = default;

    void setup_using_json(nlohmann::json &data);
    void setup_using_baked_frame_json(nlohmann::json &data);
    void setup_instanced_vbo();
    std::shared_ptr<CelestialBody>
    add_celestial_body(double mass, glm::vec3 pos, glm::vec3 vel);
    void build_octree();
    void normal_algorithm(double dt);
    void barnes_hut_algorithm(double dt);
    std::vector<std::shared_ptr<CelestialBody>> celestial_bodies() const;

    void bind_shader(const axolote::gl::Shader &shader_program) override;
    axolote::gl::Shader get_shader() const override;
    void update(double dt) override;
    void draw() override;
    void draw(const glm::mat4 &mat) override;

private:
    axolote::gl::VBO instancedVBO;
    std::vector<std::shared_ptr<CelestialBody>> _celestial_bodies;
};
