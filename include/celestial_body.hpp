#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <axolote/engine.hpp>

#include <constants.hpp>

class CelestialBody : public axolote::Entity
{
public:
    double mass;
    glm::vec3 velocity;
    glm::vec3 pos;
    bool is_black_hole;

    CelestialBody(
        double mass, const glm::vec3 &velocity, const glm::vec3 &pos,
        bool is_black_hole
    );

    glm::vec3 calculate_acceleration_vec(const CelestialBody &other);
    void update(double dt) override;
    void draw() override;
};
