#include <celestial_body.hpp>

CelestialBody::CelestialBody(
    double mass, const glm::vec3 &velocity, const glm::vec3 &pos
) :
    mass{mass},
    velocity{velocity},
    pos{pos}
{
}

glm::vec3 CelestialBody::calculate_acceleration_vec(const CelestialBody &other)
{
    glm::vec3 direction = glm::normalize(pos - other.pos);
    double r = glm::distance(pos, other.pos);
    float gravitational_acceleration = (G * mass) / (r * r);
    return direction * gravitational_acceleration;
}

void CelestialBody::update(double dt)
{
    pos += velocity * (float)dt;
    glm::mat4 mat = glm::translate(glm::mat4{1.0f}, pos);
    set_matrix(0, mat);
}

void CelestialBody::draw()
{
    Entity::draw();
};
