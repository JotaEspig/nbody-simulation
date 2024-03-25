#pragma once

#include <memory>

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
        glm::vec3 cube_end;
        std::shared_ptr<CelestialBody> body;
        glm::vec3 center_of_mass;
        double total_mass = false;
        bool is_leaf = false;

        std::unique_ptr<Node> ulf; // Up left front
        std::unique_ptr<Node> ulb; // Up left back
        std::unique_ptr<Node> urf; // Up right front
        std::unique_ptr<Node> urb; // Up right back
        std::unique_ptr<Node> blf; // Bottom left front
        std::unique_ptr<Node> blb; // Bottom left back
        std::unique_ptr<Node> brf; // Bottom right front
        std::unique_ptr<Node> brb; // Bottom right back

        Node();
        Node(const std::shared_ptr<CelestialBody> &body);

        void insert(const std::shared_ptr<CelestialBody> &body);
        // Turns a leaf node into a normal node with 4 leafs (where one of them
        // is the previous leaf node)
        void split();
    };

    std::unique_ptr<Node> root;
    double max_x, max_y, max_z;

    OcTree();

    void insert(const std::shared_ptr<CelestialBody> &body);
    static std::unique_ptr<Node> leaf();
    static std::unique_ptr<Node> leaf(const std::shared_ptr<CelestialBody> &body
    );
};
