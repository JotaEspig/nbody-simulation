#include <cstddef>
#include <memory>
#include <vector>

#include <axolote/glad/glad.h>
#include <axolote/object3d.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include <nlohmann/json.hpp>

#include "celestial_body_system.hpp"
#include "octree.hpp"

#define UNUSED(x) (void)(x)

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
        model_matrices.push_back(_celestial_bodies[i]->mat);
    }

    std::vector<glm::vec4> colors;
    for (std::size_t i = 0; i < amount; ++i) {
        colors.push_back({_celestial_bodies[i]->color(), 1.0f});
    }

    std::shared_ptr<axolote::gl::VAO> vao = sphere.vao;
    vao->bind();

    // Colors VBO
    instanced_colors_vbo->bind();
    instanced_colors_vbo->buffer_data(
        colors.size() * sizeof(glm::vec4), colors.data(), GL_DYNAMIC_DRAW
    );
    vao->link_attrib(instanced_colors_vbo, 1, 4, GL_FLOAT, 0, (void *)0);
    vao->attrib_divisor(instanced_colors_vbo, 1, 1);
    instanced_colors_vbo->unbind();

    // Instanced Matrices VBO
    GLsizeiptr vec4_size = sizeof(glm::vec4);
    GLsizeiptr mat4_size = sizeof(glm::mat4);

    instanced_matrices_vbo->bind();
    instanced_matrices_vbo->buffer_data(
        model_matrices.size() * sizeof(glm::mat4), model_matrices.data(),
        GL_DYNAMIC_DRAW
    );
    vao->link_attrib(
        instanced_matrices_vbo, 4, 4, GL_FLOAT, mat4_size, (void *)0
    );
    vao->link_attrib(
        instanced_matrices_vbo, 5, 4, GL_FLOAT, mat4_size, (void *)(vec4_size)
    );
    vao->link_attrib(
        instanced_matrices_vbo, 6, 4, GL_FLOAT, mat4_size,
        (void *)(2 * vec4_size)
    );
    vao->link_attrib(
        instanced_matrices_vbo, 7, 4, GL_FLOAT, mat4_size,
        (void *)(3 * vec4_size)
    );

    vao->attrib_divisor(instanced_matrices_vbo, 4, 1);
    vao->attrib_divisor(instanced_matrices_vbo, 5, 1);
    vao->attrib_divisor(instanced_matrices_vbo, 6, 1);
    vao->attrib_divisor(instanced_matrices_vbo, 7, 1);
    vao->unbind();

    update_vbos();
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

void CelestialBodySystem::naive_algorithm(double dt) {
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

void CelestialBodySystem::update_vbos() {
    std::vector<glm::mat4> model_matrices;
    std::vector<glm::vec4> colors;
    for (auto &c : _celestial_bodies) {
        c->update_values();
        model_matrices.push_back(c->mat);
        colors.push_back({c->color(), 1.0f});
    }
    instanced_colors_vbo->bind();
    glBufferSubData(
        GL_ARRAY_BUFFER, 0, colors.size() * sizeof(glm::vec4), colors.data()
    );
    instanced_colors_vbo->unbind();

    instanced_matrices_vbo->bind();
    glBufferSubData(
        GL_ARRAY_BUFFER, 0, model_matrices.size() * sizeof(glm::mat4),
        model_matrices.data()
    );
    instanced_matrices_vbo->unbind();
}

void CelestialBodySystem::bind_shader(
    std::shared_ptr<axolote::gl::Shader> shader_program
) {
    sphere.bind_shader(shader_program);
}

std::vector<std::shared_ptr<axolote::gl::Shader>>
CelestialBodySystem::get_shaders() const {
    return sphere.get_shaders();
}

void CelestialBodySystem::update(double dt) {
    // naive_algorithm(dt);
    barnes_hut_algorithm(dt);

    update_vbos();
}

void CelestialBodySystem::draw() {
    get_shaders()[0]->activate();
    sphere.vao->bind();
    glDrawElementsInstanced(
        GL_TRIANGLES, sphere.indices().size(), GL_UNSIGNED_INT, 0,
        _celestial_bodies.size()
    );
    sphere.vao->unbind();
}

void CelestialBodySystem::draw(const glm::mat4 &mat) {
    UNUSED(mat);
}
