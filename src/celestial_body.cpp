#include <celestial_body.hpp>

CelestialBody::CelestialBody(double mass, const glm::vec3 &velocity) :
    mass{mass},
    velocity{velocity}
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
    glm::mat4 mat
        = glm::translate(objects[0]->get_matrix(), velocity * (float)dt);
    set_matrix(0, mat);
}

void CelestialBody::draw()
{
    for (auto &e : objects)
    {
        for (auto &e2 : e->meshes)
        {
            e2.shader.activate();
            e2.shader.set_uniform_int("is_light_color_set", !is_light_emissor);
        }
    }

    Entity::draw();

    for (auto &e : objects)
    {
        for (auto &e2 : e->meshes)
        {
            e2.shader.activate();
            e2.shader.set_uniform_int("is_light_color_set", 0);
        }
    }
};
