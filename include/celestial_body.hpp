#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <axolote/engine.hpp>

#include <constants.hpp>

class CelestialBody : public axolote::Entity
{
public:
    bool is_light_emissor = false;
    double mass;
    glm::vec3 velocity;
    glm::vec3 pos;

    CelestialBody(double mass, const glm::vec3 &velocity);

    glm::vec3 calculate_acceleration_vec(const CelestialBody &other);
    void update(double dt) override;
    void draw() override;
};
