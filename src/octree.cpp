#include <memory>

#include "celestial_body.hpp"
#include <octree.hpp>

OcTree::Node::Node()
{
}

OcTree::Node::Node(const std::shared_ptr<CelestialBody> &body) :
    body{body}
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
}

std::unique_ptr<OcTree::Node> &
OcTree::Node::find_correct_child(const glm::vec3 &pos)
{
    glm::vec3 mid = (cube_start + cube_end) / 2.0f;
    if (pos.x > mid.x)
    {
        if (pos.y > mid.y)
        {
            if (pos.z > mid.z)
                return rub;
            else
                return ruf;
        }
        else
        {
            if (pos.z > mid.z)
                return rub;
            else
                return ruf;
        }
    }
    else
    {
        if (pos.y > mid.y)
        {
            if (pos.z > mid.z)
                return lub;
            else
                return luf;
        }
        else
        {
            if (pos.z > mid.z)
                return lub;
            else
                return luf;
        }
    }
}

void OcTree::insert(const std::shared_ptr<CelestialBody> &body)
{
    if (root == nullptr)
    {
        root = leaf(body);
        root->cube_start = glm::vec3{-max_x, -max_y, -max_z};
        root->cube_end = glm::vec3{max_x, max_y, max_z};
    }
    else
    {
        root->insert(body);
    }
}

std::unique_ptr<OcTree::Node> OcTree::leaf()
{
    auto node = std::make_unique<Node>();
    node->is_leaf = true;
    return node;
}

std::unique_ptr<OcTree::Node>
OcTree::leaf(const std::shared_ptr<CelestialBody> &body)
{
    auto node = std::make_unique<Node>(body);
    node->is_leaf = true;
    return node;
}
