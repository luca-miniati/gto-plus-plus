#pragma once

#include <memory>
#include <vector>

class Node;

using NodePtr = std::shared_ptr<Node>;

class Node {
public:
    bool is_terminal();
    std::vector<NodePtr> get_children();
};
