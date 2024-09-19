#include <algorithm>
#include <cmath>

#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "celestial_body.hpp"
#include "constants.hpp"

#define UNUSED(x) (void)(x)
#define BASE_MASS_INTERPOLATION 200.0f
#define START_COLOR        \
    glm::vec3 {            \
        0.0f, 0.749f, 1.0f \
    }
#define END_COLOR           \
    glm::vec3 {             \
        1.0f, 0.4549f, 0.0f \
    }
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

void CelestialBody::collide(std::shared_ptr<CelestialBody> other) {
    // If the two bodies are way too close to each other, they are merged
    if (should_merge(other)) {
        float new_mass = mass() + other->mass();
        pos = (pos * (float)mass() + other->pos * (float)other->mass())
              / new_mass;
        velocity = (velocity * (float)mass()
                    + other->velocity * (float)other->mass())
                   / new_mass;
        set_mass(new_mass);
        other->merged = true;
    }
    else {
        glm::vec3 dir = glm::normalize(pos - other->pos);
        float dist = glm::length(pos - other->pos);

        float inv_mass_a = 1 / (mass() + 1);
        float inv_mass_b = 1 / (other->mass() + 1);
        float r = (radius() + other->radius()) / 2;
        glm::vec3 mtd = dir * ((2 * r - dist) * inv_mass_a / (inv_mass_a + inv_mass_b));
        pos += mtd;

        float impact_speed = glm::dot(velocity - other->velocity, dir);
        glm::vec3 force = dir * (impact_speed * 0.5f);
        velocity -= force;
    }
}

bool CelestialBody::should_merge(std::shared_ptr<CelestialBody> other) const {
    bool is_massive_enough = std::max(mass(), other->mass()) >= 50.0f;
    bool is_close_enough = glm::distance(pos, other->pos) < std::max(radius(), other->radius()) * 0.1f;
    return is_massive_enough || is_close_enough;
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
    _color = COLOR_INTERPOLATION(
        START_COLOR, END_COLOR, _mass / BASE_MASS_INTERPOLATION
    );
    update_matrix();
}
