#include <cstdlib>
#include <iostream>
#include <memory>
#include <ostream>
#include <utility>

#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#include "celestial_body.hpp"
#include "octree.hpp"

// ---- OCTREE NODE ----

OcTree::Node::Node()
{
}

OcTree::Node::Node(glm::vec3 cube_start, float width) :
    cube_start{cube_start},
    width{width}

{
}

void OcTree::Node::insert(const std::shared_ptr<CelestialBody> &body)
{
    float m1 = total_mass;
    float m2 = body->mass;
    glm::vec3 x1 = center_of_mass;
    glm::vec3 x2 = body->pos;

    if (is_leaf)
    {
        if (Node::body == nullptr)
        {
            center_of_mass = x2;
            total_mass = m2;
            Node::body = body;
            return;
        }

        // split must be the first!
        split();
        center_of_mass = (m1 * x1 + m2 * x2) / (m1 + m2);
        total_mass += m2;

        auto &correct_node = find_correct_child(x2);
        correct_node->insert(body);
        return;
    }

    center_of_mass = (m1 * x1 + m2 * x2) / (m1 + m2);
    total_mass += m2;

    auto &correct_node = find_correct_child(x2);
    correct_node->insert(body);
}

void OcTree::Node::split()
{
    if (!is_leaf || body == nullptr)
        return;

    float new_width = width * 0.5f;
    luf = std::make_unique<Node>(
        glm::vec3{
            cube_start.x, cube_start.y + new_width, cube_start.z + new_width
        },
        new_width
    );
    lub = std::make_unique<Node>(
        glm::vec3{cube_start.x, cube_start.y + new_width, cube_start.z},
        new_width
    );
    lbf = std::make_unique<Node>(
        glm::vec3{cube_start.x, cube_start.y, cube_start.z + new_width},
        new_width
    );
    lbb = std::make_unique<Node>(
        glm::vec3{cube_start.x, cube_start.y, cube_start.z}, new_width
    );
    ruf = std::make_unique<Node>(
        glm::vec3{
            cube_start.x + new_width, cube_start.y + new_width,
            cube_start.z + new_width
        },
        new_width
    );
    rub = std::make_unique<Node>(
        glm::vec3{
            cube_start.x + new_width, cube_start.y + new_width, cube_start.z
        },
        new_width
    );
    rbf = std::make_unique<Node>(
        glm::vec3{
            cube_start.x + new_width, cube_start.y, cube_start.z + new_width
        },
        new_width
    );
    rbb = std::make_unique<Node>(
        glm::vec3{cube_start.x + new_width, cube_start.y, cube_start.z},
        new_width
    );

    auto &correct_node = find_correct_child(body->pos);
    std::swap(correct_node->body, body);
    correct_node->center_of_mass = center_of_mass;
    correct_node->total_mass = total_mass;

    is_leaf = false;
}

std::unique_ptr<OcTree::Node> &
OcTree::Node::find_correct_child(const glm::vec3 &pos)
{
    glm::vec3 mid = cube_start + width * 0.5f;
    std::cout << "FIND" << std::endl;
    std::cout << glm::to_string(cube_start) << " - " << width << std::endl;
    std::cout << glm::to_string(mid) << std::endl;
    std::cout << glm::to_string(pos) << std::endl;
    if (pos.x > mid.x)
    {
        if (pos.y > mid.y)
        {
            if (pos.z > mid.z)
                return ruf;
            else
                return rub;
        }
        else
        {
            if (pos.z > mid.z)
                return rbf;
            else
                return rbb;
        }
    }
    else
    {
        if (pos.y > mid.y)
        {
            if (pos.z > mid.z)
                return luf;
            else
                return lub;
        }
        else
        {
            if (pos.z > mid.z)
                return lbf;
            else
                return lbb;
        }
    }
}

glm::vec3 OcTree::Node::center() const
{
    return cube_start + width * 0.5f;
}

bool OcTree::Node::should_use_this_node() const
{
    return is_leaf
           || glm::distance(center_of_mass, center())
                  < width / OcTree::simulation_precision;
}

std::ostream &operator<<(std::ostream &os, OcTree::Node node)
{
    std::cout << "[ " << glm::to_string(node.cube_start) << ", " << node.width
              << ", " << node.body.get() << ", "
              << glm::to_string(node.center_of_mass) << ", " << node.total_mass
              << ", " << node.is_leaf << " ]" << std::endl;

    auto x = node.luf.get();
    std::cout << "luf -> " << x << " - ";
    if (x)
        std::cout << node.luf->body << std::endl;
    x = node.lub.get();
    std::cout << "lub -> " << x << " - ";
    if (x)
        std::cout << node.lub->body << std::endl;
    x = node.lbf.get();
    std::cout << "lbf -> " << x << " - ";
    if (x)
        std::cout << node.lbf->body << std::endl;
    x = node.lbb.get();
    std::cout << "lbb -> " << x << " - ";
    if (x)
        std::cout << node.lbb->body << std::endl;
    x = node.ruf.get();
    std::cout << "ruf -> " << x << " - ";
    if (x)
        std::cout << node.ruf->body << std::endl;
    x = node.rub.get();
    std::cout << "rub -> " << x << " - ";
    if (x)
        std::cout << node.rub->body << std::endl;
    x = node.rbf.get();
    std::cout << "rbf -> " << x << " - ";
    if (x)
        std::cout << node.rbf->body << std::endl;
    x = node.rbb.get();
    std::cout << "rbb -> " << x << " - ";
    if (x)
        std::cout << node.rbb->body << std::endl;
    return os;
}

std::ostream &operator<<(std::ostream &os, std::unique_ptr<OcTree::Node> &node)
{
    if (node == nullptr)
        return os;

    std::cout << "[ " << glm::to_string(node->cube_start) << ", " << node->width
              << ", " << node->body.get() << ", "
              << glm::to_string(node->center_of_mass) << ", "
              << node->total_mass << ", " << node->is_leaf << " ]" << std::endl;

    auto x = node->luf.get();
    std::cout << "luf -> " << x << " - ";
    if (x)
        std::cout << node->luf->body << std::endl;
    x = node->lub.get();
    std::cout << "lub -> " << x << " - ";
    if (x)
        std::cout << node->lub->body << std::endl;
    x = node->lbf.get();
    std::cout << "lbf -> " << x << " - ";
    if (x)
        std::cout << node->lbf->body << std::endl;
    x = node->lbb.get();
    std::cout << "lbb -> " << x << " - ";
    if (x)
        std::cout << node->lbb->body << std::endl;
    x = node->ruf.get();
    std::cout << "ruf -> " << x << " - ";
    if (x)
        std::cout << node->ruf->body << std::endl;
    x = node->rub.get();
    std::cout << "rub -> " << x << " - ";
    if (x)
        std::cout << node->rub->body << std::endl;
    x = node->rbf.get();
    std::cout << "rbf -> " << x << " - ";
    if (x)
        std::cout << node->rbf->body << std::endl;
    x = node->rbb.get();
    std::cout << "rbb -> " << x << " - ";
    if (x)
        std::cout << node->rbb->body << std::endl;
    return os;
}

// ---- OCTREE ----

uint OcTree::simulation_precision = 1000;

OcTree::OcTree()
{
}

OcTree::OcTree(float initial_coord)
{
    if (initial_coord == 0)
        return;
    else if (initial_coord > 0)
        initial_coord = -initial_coord;

    OcTree::initial_coord = initial_coord;
    initial_width = std::abs(2 * OcTree::initial_width);
}

void OcTree::insert(const std::shared_ptr<CelestialBody> &body)
{
    if (root == nullptr)
    {
        root = std::make_unique<Node>(
            glm::vec3{initial_coord, initial_coord, initial_coord},
            initial_width
        );
        root->center_of_mass = body->pos;
        root->total_mass = body->mass;
        root->body = body;
    }
    else
    {
        root->insert(body);
    }
}
