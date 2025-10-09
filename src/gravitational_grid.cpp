#include <cmath>
#include <glm/common.hpp>
#include <memory>
#include <vector>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#include <axolote/glad/glad.h>
#include <axolote/structs.hpp>
#define DEBUG
#include <axolote/utils.hpp>

#include "celestial_body.hpp"
#include "gravitational_grid.hpp"

#define G 6.67430e-11f
#define MIN_MASS 1.0
#define MAX_MASS 1.989e30
#define A 40000.0
#define K 4000.0

GravGrid::GravGrid(std::vector<std::shared_ptr<CelestialBody>> bodies) {
    // Find the most massive body
    std::shared_ptr<CelestialBody> biggest = bodies[0];
    for (auto &c : bodies) {
        if (c->mass() > biggest->mass()) {
            biggest = c;
        }
    }

    std::shared_ptr<CelestialBody> farest = bodies[0];
    for (auto &c : bodies) {
        if (glm::length(c->pos) > glm::length(farest->pos)) {
            farest = c;
        }
    }
    //// Calculate mean distance between two bodies
    // double total_distance = 0.0;
    // int count = 0;
    // for (std::size_t i = 0; i < bodies.size(); ++i) {
    //     for (std::size_t j = 0; j < bodies.size(); ++j) {
    //         if (i == j)
    //             continue;

    //        total_distance += glm::distance(bodies[i]->pos, bodies[j]->pos);
    //        ++count;
    //    }
    //}
    // if (count < 40) {
    //    count = 40;
    //}

    // axolote::debug(
    //     axolote::DebugType::INFO2, "Total distance: %lf, count: %d",
    //     total_distance, count
    //);
    // double mean_distance_from_two_bodies = total_distance / count;
    // axolote::debug(
    //     axolote::DebugType::INFO2, "Mean distance between two bodies: %f",
    //     mean_distance_from_two_bodies
    //);

    // int size = glm::length(farest->pos) * 2;        // size of the grid
    // int width = (int)mean_distance_from_two_bodies; // spacing between lines
    // axolote::debug(
    //     axolote::DebugType::INFO2,
    //     "Creating gravitational grid of size %d x "
    //     "%d with spacing %d",
    //     size, size, width
    //);

    int size = (int)glm::length(farest->pos) * 1.6;
    int width = size / 40;

    // Calculate the multiplier constant based on the most massive body
    multiplier_constant = calculate_multiplier(biggest->mass());
    axolote::debug(
        axolote::DebugType::INFO2, "Chosen multiplier: %lf", multiplier_constant
    );

    std::vector<axolote::Vertex> vertices;
    _indices.clear();

    // build a 2d mesh of quads
    for (int i = -size / 2; i < size / 2; i += width) {
        for (int j = -size / 2; j < size / 2; j += width) {
            vertices.push_back(
                axolote::Vertex{
                    {(float)i, 0.0f, (float)j}, {1.0f, 1.0f, 1.0f, 1.0f}
                }
            );
        }
    }

    // build indices vector for lines
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

    // Initialize displacements vector and SSBO
    _displacements.clear();
    _displacements.resize(_indices.size());

    glGenBuffers(1, &ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    glBufferData(
        GL_SHADER_STORAGE_BUFFER, sizeof(float) * _displacements.capacity(),
        (void *)0, GL_DYNAMIC_DRAW
    );
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);
}

void GravGrid::update_for_body(std::shared_ptr<CelestialBody> c) {
    // O(n²)
    for (std::size_t i = 0; i < _displacements.capacity(); ++i) {
        glm::vec3 vertex_pos = gmodel->meshes[0].vertices[i].pos;

        glm::vec3 pos = c->pos;
        // dividing by X increases the area of "perception" of gravity
        float dist = glm::distance(vertex_pos, pos) / 10.0f;

        // Do not allow division by zero or rs tending to infinity
        dist = std::max(dist, 0.05f);
        double rs = (2 * G * c->mass()) / (dist * dist);
        double w = 2 * std::sqrt(rs * (dist - rs)) * multiplier_constant;

        _displacements[i] += w;
    }
}

void GravGrid::draw() {
    bool cull_face = glIsEnabled(GL_CULL_FACE);
    glDisable(GL_CULL_FACE);

    gmodel->meshes[0].default_draw_binds(_model_matrix);
    gmodel->meshes[0].vao()->bind();

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo); // bind ssbo
    glDrawElements(GL_LINES, _indices.size(), GL_UNSIGNED_INT, 0);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); // unbind ssbo

    gmodel->meshes[0].vao()->unbind();
    gmodel->meshes[0].default_draw_unbinds();

    if (cull_face) {
        glEnable(GL_CULL_FACE);
    }
}

double GravGrid::calculate_multiplier(double max_mass) {
    max_mass = glm::clamp(max_mass, MIN_MASS, MAX_MASS);
    double multiplier = A * exp(-max_mass / K) + 1;
    return multiplier;
}
