#include "node.h"

bool Node::is_terminal() { return false; }

std::vector<NodePtr> Node::get_children() { return {}; }
