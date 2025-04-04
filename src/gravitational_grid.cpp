#include <cmath>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#include "axolote/glad/glad.h"
#include "axolote/structs.hpp"

#include "gravitational_grid.hpp"

GravGrid::GravGrid(int size, float width) {
    using namespace axolote;

    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;

    // build a 2d mesh of quads
    for (int i = -size / 2; i < size / 2; i += width) {
        for (int j = -size / 2; j < size / 2; j += width) {
            vertices.push_back(
                Vertex{{(float)i, 0.0f, (float)j}, {1.0f, 1.0f, 1.0f, 1.0f}}
            );
        }
    }

    // indices
    int amount_per_axis = std::ceil(size / width);
    for (int i = 0; i < amount_per_axis - 1; ++i) {
        for (int j = 0; j < amount_per_axis - 1; ++j) {
            GLuint topLeft = i * amount_per_axis + j;
            GLuint topRight = i * amount_per_axis + (j + 1);
            GLuint bottomLeft = (i + 1) * amount_per_axis + j;
            GLuint bottomRight = (i + 1) * amount_per_axis + (j + 1);

            // top edge
            indices.push_back(topLeft);
            indices.push_back(topRight);

            // right edge
            indices.push_back(topRight);
            indices.push_back(bottomRight);

            // bottom edge
            indices.push_back(bottomRight);
            indices.push_back(bottomLeft);

            // left edge
            indices.push_back(bottomLeft);
            indices.push_back(topLeft);
        }
    }

    _indices_size = indices.size();

    gmodel->meshes.push_back({vertices, indices, {}});
    set_matrix(glm::mat4{1.0f});
}

void GravGrid::draw() {
    bool cull_face = glIsEnabled(GL_CULL_FACE);
    GLint polygon_mode[2];
    glGetIntegerv(GL_POLYGON_MODE, polygon_mode);

    glDisable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    gmodel->meshes[0].default_draw_binds(_model_matrix);
    glDrawElements(GL_LINES, _indices_size, GL_UNSIGNED_INT, 0);
    gmodel->meshes[0].default_draw_unbinds();

    if (cull_face) {
        glEnable(GL_CULL_FACE);
    }
    glPolygonMode(GL_FRONT_AND_BACK, polygon_mode[0]);
}
