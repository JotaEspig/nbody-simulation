#pragma once

#include "axolote/object3d.hpp"

#include "celestial_body_system.hpp"

class GravGrid : public axolote::Object3D {
public:
    GravGrid(
        std::shared_ptr<CelestialBodySystem> system, int size, float width = 10
    );

    void update(double absolute_time, double dt) override;
    void draw() override;

private:
    std::shared_ptr<CelestialBodySystem> _system;
    std::vector<GLuint> _indices;
    std::vector<float> displacements;
};
