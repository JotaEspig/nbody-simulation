#pragma once

#include "axolote/object3d.hpp"

class GravGrid : public axolote::Object3D {
public:
    GravGrid(int size, float width = 10);

    void draw() override;
private:
    GLuint _indices_size = 0;
};
