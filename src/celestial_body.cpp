#include <algorithm>
#include <cmath>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "celestial_body.hpp"
#include "constants.hpp"

#define UNUSED(x) (void)(x)

#define COLOR_INTERPOLATION(c1, c2, f)                      \
    glm::vec3 {                                             \
        c1.x + (c2.x - c1.x) * f, c1.y + (c2.y - c1.y) * f, \
            c1.z + (c2.z - c1.z) * f                        \
    }

CelestialBody::CelestialBody(
    double mass, const glm::vec3 &velocity, const glm::vec3 &pos
) :
  velocity{velocity},
  pos{pos} {
    set_mass(mass);
    update_matrix();
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
    _radius = std::max(0.5, std::log2(_mass) / 2.0f);
}

float CelestialBody::radius() const {
    return _radius;
}

glm::vec3 CelestialBody::color() const {
    return _color;
}

void CelestialBody::update_matrix() {
    mat = glm::translate(glm::mat4{1.0f}, pos);
    mat = glm::scale(mat, glm::vec3{_radius, _radius, _radius});
}

void CelestialBody::update_values() {
    double aux_mass = 200.0;
    glm::vec3 start_color{0.0f, 0.749f, 1.0f};
    glm::vec3 end_color{1.0f, 0.4549f, 0.0f};
    _color = COLOR_INTERPOLATION(start_color, end_color, _mass / aux_mass);
    update_matrix();
}
