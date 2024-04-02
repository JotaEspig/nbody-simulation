#include <cstdlib>
#include <iostream>
#include <memory>
#include <ostream>
#include <utility>

#include <glm/fwd.hpp>
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
    if (is_leaf)
    {
        split();
    }
}

void OcTree::Node::split()
{
    if (!is_leaf)
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
    // see what to do with center_of_mass and total_mass

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

std::ostream &operator<<(std::ostream &os, OcTree::Node node)
{
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
        root->body = body;
    }
    else
    {
        root->insert(body);
    }
}
