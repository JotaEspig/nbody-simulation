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
    // Bodies that are momentarily separating (or exactly co-moving) have
    // nothing to resolve this frame: gravity will bring them back together
    // (or apart) on a later frame, at which point this is re-evaluated.
    double sep_dist = glm::distance(pos, other->pos);
    if (sep_dist >= COLLISION_EPS) {
        glm::vec3 sep_n = (pos - other->pos) / (float)sep_dist;
        if (glm::dot(velocity - other->velocity, sep_n) >= 0.0f)
            return;
    }

    // If the two bodies are gravitationally bound at contact, they merge
    // (perfectly inelastic collision, conserving mass and momentum)
    if (should_merge(other)) {
        double new_mass = mass() + other->mass();
        pos = (pos * (float)mass() + other->pos * (float)other->mass())
              / (float)new_mass;
        velocity = (velocity * (float)mass()
                    + other->velocity * (float)other->mass())
                   / (float)new_mass;
        set_mass(new_mass);
        other->merged = true;
        return;
    }

    // Otherwise, this is a high-energy "hit-and-run" collision: the bodies
    // exchange momentum via an impulse whose restitution is derived from
    // how much the impact speed exceeds the mutual escape velocity (0 at
    // the escape-velocity threshold, growing above it), capped at
    // MAX_RESTITUTION. Real rocky/icy bodies never bounce off each other
    // perfectly elastically, regardless of impact speed -- the material
    // deforms/fractures and dissipates energy as heat, so a physically
    // realistic restitution stays well below 1 even far above escape speed.
    double dist = glm::distance(pos, other->pos);
    glm::vec3 n = (pos - other->pos) / (float)dist;
    double v_n = glm::dot(velocity - other->velocity, n);
    double v_esc = mutual_escape_velocity(*other);
    double e
        = std::sqrt(std::max(0.0, v_n * v_n - v_esc * v_esc)) / std::abs(v_n);
    e = std::min(e, MAX_RESTITUTION);

    double inv_m1 = 1.0 / mass();
    double inv_m2 = 1.0 / other->mass();
    double j = -(1.0 + e) * v_n / (inv_m1 + inv_m2);

    velocity += (float)(j * inv_m1) * n;
    other->velocity -= (float)(j * inv_m2) * n;
}

bool CelestialBody::should_merge(std::shared_ptr<CelestialBody> other) const {
    double dist = glm::distance(pos, other->pos);
    if (dist < COLLISION_EPS)
        return true;

    glm::vec3 n = (pos - other->pos) / (float)dist;
    float v_n = glm::dot(velocity - other->velocity, n);
    if (v_n >= 0.0f)
        return false;

    double v_esc = mutual_escape_velocity(*other);
    return std::abs(v_n) <= v_esc;
}

double CelestialBody::mutual_escape_velocity(const CelestialBody &other) const {
    double r_sum = radius() + other.radius();
    return std::sqrt(2.0 * G * (mass() + other.mass()) / r_sum);
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
