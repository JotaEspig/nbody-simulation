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
}

void OcTree::insert(const std::shared_ptr<CelestialBody> &body)
{
    if (root == nullptr)
    {
        root = leaf(body);
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
