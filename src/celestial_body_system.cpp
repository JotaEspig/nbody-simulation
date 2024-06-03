#include <cstddef>
#include <iostream>
#include <memory>
#include <vector>

#include <axolote/glad/glad.h>
#include <axolote/object3d.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include <nlohmann/json.hpp>

#include "celestial_body_system.hpp"
#include "octree.hpp"

void CelestialBodySystem::setup_using_json(nlohmann::json &data) {
    using json = nlohmann::json;

    _celestial_bodies.clear();
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
        add_body(e["mass"], pos, vel);
    }
}

void CelestialBodySystem::setup_using_baked_frame_json(nlohmann::json &data) {
    _celestial_bodies.clear();
    for (auto &e : data) {
        double mass = e["m"];
        glm::vec3 pos;
        glm::vec3 vel{0.0f, 0.0f, 0.0f};
        std::string xstr = e["px"];
        std::string ystr = e["py"];
        std::string zstr = e["pz"];
        pos.x = std::stof(xstr);
        pos.y = std::stof(ystr);
        pos.z = std::stof(zstr);
        add_body(mass, pos, vel);
    }
}

void CelestialBodySystem::setup_instanced_vbo() {
    std::size_t amount = _celestial_bodies.size();
    std::vector<glm::mat4> model_matrices;
    for (std::size_t i = 0; i < amount; ++i) {
        std::cout << glm::to_string(_celestial_bodies[i]->mat) << std::endl;
        model_matrices.push_back(_celestial_bodies[i]->mat);
    }

    std::vector<glm::vec3> colors;
    for (std::size_t i = 0; i < amount; ++i) {
        colors.push_back(_celestial_bodies[i]->color());
    }

    axolote::gl::VAO vao = sphere.vao;
    vao.bind();

    // Colors VBO
    sphere.colors_vbo.bind();
    glBufferSubData(
        GL_ARRAY_BUFFER, 0, colors.size() * sizeof(glm::vec3), colors.data()
    );
    vao.attrib_divisor(sphere.colors_vbo, 1, 1);
    sphere.colors_vbo.bind();

    // Instanced Matrices VBO
    glGenBuffers(1, &instanced_matrices_vbo.id);
    instanced_matrices_vbo.bind();
    glBufferData(
        GL_ARRAY_BUFFER, model_matrices.size() * sizeof(glm::mat4),
        model_matrices.data(), GL_DYNAMIC_DRAW
    );

    GLsizeiptr vec4_size = sizeof(glm::vec4);
    GLsizeiptr mat4_size = sizeof(glm::mat4);
    vao.link_attrib(
        instanced_matrices_vbo, 4, 4, GL_FLOAT, mat4_size, (void *)0
    );
    vao.link_attrib(
        instanced_matrices_vbo, 5, 4, GL_FLOAT, mat4_size, (void *)(vec4_size)
    );
    vao.link_attrib(
        instanced_matrices_vbo, 6, 4, GL_FLOAT, mat4_size,
        (void *)(2 * vec4_size)
    );
    vao.link_attrib(
        instanced_matrices_vbo, 7, 4, GL_FLOAT, mat4_size,
        (void *)(3 * vec4_size)
    );

    vao.attrib_divisor(instanced_matrices_vbo, 4, 1);
    vao.attrib_divisor(instanced_matrices_vbo, 5, 1);
    vao.attrib_divisor(instanced_matrices_vbo, 6, 1);
    vao.attrib_divisor(instanced_matrices_vbo, 7, 1);
    vao.unbind();
}

std::shared_ptr<CelestialBody>
CelestialBodySystem::add_body(double mass, glm::vec3 pos, glm::vec3 vel) {
    std::shared_ptr<CelestialBody> body{new CelestialBody{mass, vel, pos}};
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
            body0->pos += body0->velocity * (float)dt;
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
            c->pos += c->velocity * (float)dt;
        }
    }

    _celestial_bodies = std::move(active_bodies);
}

std::vector<std::shared_ptr<CelestialBody>>
CelestialBodySystem::celestial_bodies() const {
    return _celestial_bodies;
}

void CelestialBodySystem::bind_shader(const axolote::gl::Shader &shader_program
) {
    sphere.bind_shader(shader_program);
}

axolote::gl::Shader CelestialBodySystem::get_shader() const {
    return sphere.get_shader();
}

void CelestialBodySystem::update(double dt) {
    // normal_algorithm(dt);
    barnes_hut_algorithm(dt);

    std::vector<glm::mat4> model_matrices;
    std::vector<glm::vec3> colors;
    for (auto &c : _celestial_bodies) {
        c->update_values(dt);
        model_matrices.push_back(c->mat);
        colors.push_back(c->color());
    }

    sphere.colors_vbo.bind();
    glBufferSubData(
        GL_ARRAY_BUFFER, 0, colors.size() * sizeof(glm::vec3), colors.data()
    );
    sphere.colors_vbo.unbind();

    instanced_matrices_vbo.bind();
    glBufferSubData(
        GL_ARRAY_BUFFER, 0, model_matrices.size() * sizeof(glm::mat4),
        model_matrices.data()
    );
    instanced_matrices_vbo.unbind();
}

void CelestialBodySystem::draw() {
    get_shader().activate();
    axolote::gl::VAO vao = sphere.vao;
    vao.bind();
    glDrawElementsInstanced(
        GL_TRIANGLES, sphere.indices().size(), GL_UNSIGNED_INT, 0,
        _celestial_bodies.size()
    );
    vao.unbind();
}

void CelestialBodySystem::draw(const glm::mat4 &mat) {
}
