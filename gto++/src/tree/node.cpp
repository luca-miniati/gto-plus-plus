#include "tree/node.h"

Node::Node(NodeIdx idx, int num_actions, std::vector<NodeIdx> children,
    bool is_chance_node, bool is_terminal)
  : idx_(idx),
    num_actions_(num_actions),
    children_(children),
    is_chance_node_(is_chance_node),
    is_terminal_(is_terminal)
{}

bool Node::IsTerminal() {
  return this->is_terminal_;
}

bool Node::IsChanceNode() {
  return this->is_chance_node_;
}

double Node::GetUtility(Deck deck) {
  return 0.0;
}

int Node::GetNumActions() {
  return this->num_actions_;
}

std::vector<NodeIdx> Node::GetChildren() {
  return this->children_;
}

NodeIdx Node::GetChild(int action_idx) {
  return this->children_[action_idx];
}
