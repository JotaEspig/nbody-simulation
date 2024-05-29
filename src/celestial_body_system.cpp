#include <cstddef>
#include <memory>

#include <axolote/glad/glad.h>
#include <axolote/object3d.hpp>
#include <nlohmann/json.hpp>
#include <vector>

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
        add_celestial_body(e["mass"], pos, vel);
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
        add_celestial_body(mass, pos, vel);
    }
}

void CelestialBodySystem::setup_instanced_vbo() {
    std::size_t amount = _celestial_bodies.size();
    std::vector<glm::mat4> model_matrices;
    for (std::size_t i = 0; i < amount; ++i) {
        glm::mat4 mat{1.0f};
        mat = glm::translate(mat, glm::vec3(i, 0, 0));
        model_matrices.push_back(mat);
    }
    //    std::vector<glm::vec3> bleee;
    //    for (std::size_t i = 0; i < amount; ++i) {
    //        glm::vec3 v(i * 2, i * 2, i * 2);
    //        bleee.push_back(v);
    //    }

    instancedVBO = axolote::gl::VBO{model_matrices};
    GLsizeiptr vec4_size = sizeof(glm::vec4);
    for (std::size_t i = 0; i < gmodel->meshes.size(); ++i) {
        axolote::gl::VAO vao = gmodel->meshes[i].vao;
        vao.bind();
        vao.link_attrib(instancedVBO, 4, 4, GL_FLOAT, vec4_size, (void *)0);
        vao.link_attrib(
            instancedVBO, 5, 4, GL_FLOAT, vec4_size, (void *)(vec4_size)
        );
        vao.link_attrib(
            instancedVBO, 6, 4, GL_FLOAT, vec4_size, (void *)(2 * vec4_size)
        );
        vao.link_attrib(
            instancedVBO, 7, 4, GL_FLOAT, vec4_size, (void *)(3 * vec4_size)
        );

        vao.attrib_divisor(instancedVBO, 4, 1);
        vao.attrib_divisor(instancedVBO, 5, 1);
        vao.attrib_divisor(instancedVBO, 6, 1);
        vao.attrib_divisor(instancedVBO, 7, 1);
        vao.unbind();
    }
}

std::shared_ptr<CelestialBody> CelestialBodySystem::add_celestial_body(
    double mass, glm::vec3 pos, glm::vec3 vel
) {
    // Create object matrix
    glm::mat4 mat{1.0f};
    mat = glm::translate(mat, pos);

    // Create body
    std::shared_ptr<CelestialBody> body{new CelestialBody{mass, vel, pos}};

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
    gmodel->bind_shader(shader_program);
}

axolote::gl::Shader CelestialBodySystem::get_shader() const {
    return gmodel->get_shader();
}

void CelestialBodySystem::update(double dt) {
    // normal_algorithm(dt);
    barnes_hut_algorithm(dt);

    for (auto &c : _celestial_bodies) {
        c->update_values(dt);
    }
}

void CelestialBodySystem::draw() {
    get_shader().activate();
    for (auto &mesh : gmodel->meshes) {
        axolote::gl::VAO vao = mesh.vao;
        vao.bind();
        glDrawElementsInstanced(
            GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0,
            _celestial_bodies.size()
        );
        vao.unbind();
    }
}

void CelestialBodySystem::draw(const glm::mat4 &mat) {
}
