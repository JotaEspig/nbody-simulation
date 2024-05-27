#include <algorithm>
#include <cmath>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "celestial_body.hpp"
#include "constants.hpp"

#define UNUSED(x) (void)(x)

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

bool CelestialBody::is_colliding(const CelestialBody &other) const {
    return (_radius + other._radius) > glm::distance(pos, other.pos);
}

void CelestialBody::merge(std::shared_ptr<CelestialBody> other) {
    float new_mass = _mass + other->_mass;
    pos = ((float)_mass * pos + (float)other->_mass * other->pos) / (new_mass);
    velocity = ((float)_mass * velocity + (float)other->_mass * other->velocity)
               / (new_mass);
    set_mass(new_mass);
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
    UNUSED(dt);
    model_mat = glm::translate(glm::mat4{1.0f}, pos);
    model_mat = glm::scale(model_mat, glm::vec3{_radius, _radius, _radius});
}

void CelestialBody::draw() {
    // hardcoded mass base
    double max_mass = 200.0;
    glm::vec3 color{
        std::min(_mass / max_mass, 1.0),
        1.0f - 0.5f * std::min(_mass / max_mass, 1.0),
        1.0f - std::min(_mass / max_mass, 1.0)
    };

    for (auto &m : gmodel->meshes) {
        m.shader.set_uniform_int("is_color_uniform_set", 1);
        m.shader.set_uniform_float3("color_uniform", color.x, color.y, color.z);
    }
    Object3D::draw();
    for (auto &m : gmodel->meshes) {
        m.shader.set_uniform_int("is_color_uniform_set", 0);
    }
};
