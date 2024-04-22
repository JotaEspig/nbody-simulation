#include <algorithm>
#include <cmath>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "celestial_body.hpp"
#include "constants.hpp"

CelestialBody::CelestialBody(
    double mass, const glm::vec3 &velocity, const glm::vec3 &pos
) :
  velocity{velocity},
  pos{pos} {
    set_mass(mass);
}

glm::vec3 CelestialBody::calculate_acceleration_vec(const CelestialBody &other
) const {
    glm::vec3 direction = glm::normalize(other.pos - pos);
    double r = glm::distance(pos, other.pos);
    float gravitational_acceleration = (G * other._mass) / (r * r);
    return direction * gravitational_acceleration;
}

glm::vec3 CelestialBody::calculate_acceleration_vec(
    const glm::vec3 &pos, double mass
) const {
    glm::vec3 direction = glm::normalize(pos - CelestialBody::pos);
    double r = glm::distance(CelestialBody::pos, pos);
    float gravitational_acceleration = (G * mass) / (r * r);
    return direction * gravitational_acceleration;
}

bool CelestialBody::is_colinding(const CelestialBody &other) const {
    return (_radius + other._radius) > glm::distance(pos, other.pos);
}

void CelestialBody::merge(std::shared_ptr<CelestialBody> other) {
    set_mass(_mass + other->_mass);
    other->merged = true;
}

double CelestialBody::mass() const {
    return _mass;
}

void CelestialBody::set_mass(double mass) {
    _mass = mass;
    _radius = std::max(0.5, std::log2(_mass));
}

float CelestialBody::radius() const {
    return _radius;
}

void CelestialBody::update(double dt) {
    pos += velocity * (float)dt;
    glm::mat4 mat = glm::translate(glm::mat4{1.0f}, pos);
    mat = glm::scale(mat, glm::vec3{_radius, _radius, _radius});
    set_matrix(0, mat);
}

void CelestialBody::draw() {
    // hardcoded mass base
    double max_mass = 200.0;
    glm::vec3 color{
        std::min(_mass / max_mass, 1.0),
        1.0f - 0.5f * std::min(_mass / max_mass, 1.0),
        1.0f - std::min(_mass / max_mass, 1.0)
    };

    for (auto &o : objects) {
        for (auto &m : o.model->meshes) {
            m.shader.set_uniform_int("is_color_uniform_set", 1);
            m.shader.set_uniform_float3(
                "color_uniform", color.x, color.y, color.z
            );
        }
    }
    Entity::draw();
    for (auto &o : objects) {
        for (auto &m : o.model->meshes) {
            m.shader.set_uniform_int("is_color_uniform_set", 0);
        }
    }
};
