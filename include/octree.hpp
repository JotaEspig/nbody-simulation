/**
 * \file octree.hpp
 * \brief Octree class
 * \author João Vitor Espig (JotaEspig)
 **/
#pragma once

#include <cstdlib>
#include <memory>
#include <ostream>

#include <glm/fwd.hpp>

#include "celestial_body.hpp"

/**
 * \brief Octree class
 * \author João Vitor Espig (JotaEspig)
 *
 * Based on Barnes-Hut algorithm
 **/
class OcTree {
public:
    /**
     * \brief Node class
     * \author João Vitor Espig (JotaEspig)
     **/
    class Node {
    public:
        /** 3D point where the cube starts **/
        glm::vec3 cube_start;
        /** Width of the cube **/
        float width;
        /** Celestial body **/
        std::shared_ptr<CelestialBody> body;
        /** Center of mass of node **/
        glm::vec3 center_of_mass;
        /** Total mass of node **/
        double total_mass = 0.0;
        /** Is leaf node **/
        bool is_leaf = true;

        /** left up front **/
        std::unique_ptr<Node> luf;
        /** left up back **/
        std::unique_ptr<Node> lub;
        /** left bottom front **/
        std::unique_ptr<Node> lbf;
        /** left bottom back **/
        std::unique_ptr<Node> lbb;
        /** right up front **/
        std::unique_ptr<Node> ruf;
        /** right up back **/
        std::unique_ptr<Node> rub;
        /** right bottom front **/
        std::unique_ptr<Node> rbf;
        /** right bottom back **/
        std::unique_ptr<Node> rbb;

        /**
         * \brief Default constructor
         * \author João Vitor Espig (JotaEspig)
         **/
        Node();
        /**
         * \brief Constructor
         * \author João Vitor Espig (JotaEspig)
         * \param cube_start - 3D point where the cube starts
         * \param width - width of the cube
         **/
        Node(glm::vec3 cube_start, float width);

        /**
         * \brief Insert a body into the node
         * \author João Vitor Espig (JotaEspig)
         * \param body - celestial body
         **/
        void insert(const std::shared_ptr<CelestialBody> &body);
        /**
         * \brief split the node into 8 leafs
         * \author João Vitor Espig (JotaEspig)
         *
         * Turns a leaf node into a normal node with 8 leafs (where one of them
         * is the previous leaf node)
         **/
        void split();
        /**
         * \brief Gets the correct child node according to a position inside it
         * \author João Vitor Espig (JotaEspig)
         * \param pos - position
         * \returns correct child node
         **/
        std::unique_ptr<Node> &find_correct_child(const glm::vec3 &pos);
        /**
         * \brief Calculates the final acceleration vector on a body
         * \author João Vitor Espig (JotaEspig)
         * \param body - celestial body
         * \param dt - delta time
         * \returns total acceleration
         **/
        glm::vec3 net_acceleration_on_body(
            std::shared_ptr<CelestialBody> body, double dt
        );
        /**
         * \brief Calculates the ratio width / distance to center of mass
         * \author João Vitor Espig (JotaEspig)
         * \param pos - position
         * \returns ratio
         **/
        double ratio_width_distance(const glm::vec3 &pos) const;

        /** Overload of << operator **/
        friend std::ostream &operator<<(std::ostream &os, Node node);
        /** Overload of << operator **/
        friend std::ostream &
        operator<<(std::ostream &os, std::unique_ptr<Node> &node);

    private:
        /**
         * \brief Should the body be counted for acceleration calculation
         * \author João Vitor Espig (JotaEspig)
         * \param other - other body
         * \returns true if the body should be called
         **/
        bool should_be_called(const std::shared_ptr<CelestialBody> &other
        ) const;
    };

    /** Simulation precision parameter, a high value means a low simulation
     * accuracy but it becomes quickier, and a low value means the opposite **/
    static double theta;
    /** Initial start coordinate **/
    float initial_coord = -1000.0f;
    /** Initial width for node **/
    float initial_width = std::abs(2 * initial_coord);

    /** Root node **/
    std::unique_ptr<Node> root;

    /**
     * \brief Default constructor
     * \author João Vitor Espig (JotaEspig)
     **/
    OcTree();
    /**
     * \brief Constructor
     * \author João Vitor Espig (JotaEspig)
     * \param initial_coord - initial start coordinate
     **/
    OcTree(float initial_coord);

    /**
     * \brief Insert a body into the octree
     * \author João Vitor Espig (JotaEspig)
     * \param body - celestial body
     **/
    void insert(const std::shared_ptr<CelestialBody> &body);
    /**
     * \brief Calculates the net acceleration on a body
     * \author João Vitor Espig (JotaEspig)
     * \param body - celestial body
     * \param dt - delta time
     * \returns net acceleration
     **/
    glm::vec3
    net_acceleration_on_body(std::shared_ptr<CelestialBody> body, double dt);
};
