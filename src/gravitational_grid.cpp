#include <cmath>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#include "axolote/glad/glad.h"
#include "axolote/structs.hpp"

#include "gravitational_grid.hpp"

#define G 6.67430e-11f

GravGrid::GravGrid(
    std::shared_ptr<CelestialBodySystem> system, int size, float width
) {
    _system = system;

    std::vector<axolote::Vertex> vertices;
    _indices.clear();

    // build a 2d mesh of quads
    for (int i = -size / 2; i < size / 2; i += width) {
        for (int j = -size / 2; j < size / 2; j += width) {
            vertices.push_back(axolote::Vertex{
                {(float)i, 0.0f, (float)j}, {1.0f, 1.0f, 1.0f, 1.0f}
            });
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
            _indices.push_back(topLeft);
            _indices.push_back(topRight);

            // right edge
            _indices.push_back(topRight);
            _indices.push_back(bottomRight);

            // bottom edge
            _indices.push_back(bottomRight);
            _indices.push_back(bottomLeft);

            // left edge
            _indices.push_back(bottomLeft);
            _indices.push_back(topLeft);
        }
    }

    gmodel->meshes.push_back({vertices, _indices, {}});
    set_matrix(glm::mat4{1.0f});
}

void GravGrid::update(double absolute_time, double dt) {
    displacements.clear();
    displacements.reserve(_indices.size());

    for (std::size_t i = 0; i < displacements.capacity(); ++i) {
        glm::vec3 vertex_pos = gmodel->meshes[0].vertices[i].pos;
        double displacement = 0.0f;

        for (auto &c : _system->celestial_bodies()) {
            glm::vec3 pos = c->pos;
            // dividing by X increases the area of "perception" of gravity
            float dist = glm::distance(vertex_pos, pos) / 10.0f;

            // Do not allow division by zero or rs tending to infinity
            dist = std::max(dist, 0.8f);
            double rs = (2 * G * c->mass()) / (dist * dist);
            // Multiply by 10000 to increase the visual effect of gravity
            double w = 2 * std::sqrt(rs * (dist - rs)) * 50000;

            displacement += w;
        }

        displacements.push_back(displacement);
    }
}

void GravGrid::draw() {
    bool cull_face = glIsEnabled(GL_CULL_FACE);
    glDisable(GL_CULL_FACE);

    gmodel->meshes[0].default_draw_binds(_model_matrix);
    gmodel->meshes[0].vao()->bind();

    get_shaders()[0]->set_uniform_float(
        "displacements_count", displacements.size()
    );
    for (std::size_t i = 0; i < displacements.size(); ++i) {
        std::string name = "displacements[" + std::to_string(i) + "]";
        get_shaders()[0]->set_uniform_float(name.c_str(), displacements[i]);
    }
    glDrawElements(GL_LINES, _indices.size(), GL_UNSIGNED_INT, 0);

    gmodel->meshes[0].vao()->unbind();
    gmodel->meshes[0].default_draw_unbinds();

    if (cull_face) {
        glEnable(GL_CULL_FACE);
    }
}
