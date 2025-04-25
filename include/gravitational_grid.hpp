#pragma once

#include "axolote/object3d.hpp"

#include "celestial_body.hpp"

class GravGrid : public axolote::Object3D {
public:
    GLuint ssbo = 0;

    GravGrid(int size, float width = 10);

    void update_for_body(std::shared_ptr<CelestialBody> c);
    void draw() override;

    friend class CelestialBodySystem;

private:
    std::vector<GLuint> _indices;
    std::vector<float> _displacements;
};
