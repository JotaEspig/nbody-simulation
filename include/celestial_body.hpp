#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <axolote/engine.hpp>

class CelestialBody : public axolote::Entity
{
public:
    double mass;
    glm::vec3 velocity;
    glm::vec3 pos;
    float radius = 1.0f;

    CelestialBody(double mass, const glm::vec3 &velocity, const glm::vec3 &pos);

    glm::vec3 calculate_acceleration_vec(const CelestialBody &other) const;
    glm::vec3
    calculate_acceleration_vec(const glm::vec3 &pos, double mass) const;
    bool is_colinding(const CelestialBody &other);
    void update(double dt) override;
    void draw() override;
};
