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

        std::unique_ptr<Node> luf; // left up front
        std::unique_ptr<Node> lub; // left up back
        std::unique_ptr<Node> ruf; // right up front
        std::unique_ptr<Node> rub; // right up back
        std::unique_ptr<Node> lbf; // left bottom front
        std::unique_ptr<Node> lbb; // left bottom back
        std::unique_ptr<Node> rbf; // right bottom front
        std::unique_ptr<Node> rbb; // right bottom back

        Node();
        Node(const std::shared_ptr<CelestialBody> &body);

        void insert(const std::shared_ptr<CelestialBody> &body);
        // Turns a leaf node into a normal node with 8 leafs (where one of them
        // is the previous leaf node)
        void split();
        std::unique_ptr<Node> &find_correct_child(const glm::vec3 &pos);
    };

    std::unique_ptr<Node> root;
    float max_x, max_y, max_z;

    OcTree();

    void insert(const std::shared_ptr<CelestialBody> &body);
    static std::unique_ptr<Node> leaf();
    static std::unique_ptr<Node> leaf(const std::shared_ptr<CelestialBody> &body
    );
};
