#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <axolote/engine.hpp>

class CelestialBody {
public:
    glm::mat4 mat{1.0f};
    glm::vec3 velocity;
    glm::vec3 pos;
    bool merged = false;

    CelestialBody(double mass, const glm::vec3 &velocity, const glm::vec3 &pos);

    glm::vec3 calculate_acceleration_vec(const CelestialBody &other) const;
    glm::vec3
    calculate_acceleration_vec(const glm::vec3 &pos, double mass) const;
    bool is_colliding(const CelestialBody &other) const;
    void merge(std::shared_ptr<CelestialBody> other);
    double mass() const;
    void set_mass(double mass);
    float radius() const;
    glm::vec3 color() const;
    void update_values(double dt);

protected:
    double _mass;
    float _radius = 1.0f;
    glm::vec3 _color;
};
