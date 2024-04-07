#pragma once

#include <cstdlib>
#include <memory>
#include <ostream>

#include <glm/fwd.hpp>

#include "celestial_body.hpp"

// Based on Barnes-Hut algorithm

class OcTree
{
public:
    class Node
    {
    public:
        glm::vec3 cube_start;
        float width;
        std::shared_ptr<CelestialBody> body;
        glm::vec3 center_of_mass;
        double total_mass = 0.0;
        bool is_leaf = true;

        std::unique_ptr<Node> luf; // left up front
        std::unique_ptr<Node> lub; // left up back
        std::unique_ptr<Node> lbf; // left bottom front
        std::unique_ptr<Node> lbb; // left bottom back
        std::unique_ptr<Node> ruf; // right up front
        std::unique_ptr<Node> rub; // right up back
        std::unique_ptr<Node> rbf; // right bottom front
        std::unique_ptr<Node> rbb; // right bottom back

        Node();
        Node(glm::vec3 cube_start, float width);

        void insert(const std::shared_ptr<CelestialBody> &body);
        // Turns a leaf node into a normal node with 8 leafs (where one of them
        // is the previous leaf node)
        void split();
        std::unique_ptr<Node> &find_correct_child(const glm::vec3 &pos);
        glm::vec3 net_acceleration_on_body(
            std::shared_ptr<CelestialBody> body, double dt
        );
        /** Calculates the ratio width / distance to center of mass **/
        bool ratio_width_distance(const glm::vec3 &pos) const;

        friend std::ostream &operator<<(std::ostream &os, Node node);
        friend std::ostream &
        operator<<(std::ostream &os, std::unique_ptr<Node> &node);
    };

    static double simulation_precision;
    float initial_coord = -1000.0f;
    float initial_width = std::abs(2 * initial_coord);

    std::unique_ptr<Node> root;

    OcTree();
    OcTree(float initial_coord);

    void insert(const std::shared_ptr<CelestialBody> &body);
    glm::vec3
    net_acceleration_on_body(std::shared_ptr<CelestialBody> body, double dt);
};
