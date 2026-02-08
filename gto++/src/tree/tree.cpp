#include <stdexcept>
#include "tree/tree.h"

Tree::Tree(int max_raises, std::unique_ptr<ActionAbstraction> abst)
  : max_raises_(max_raises), 
    abst_(std::move(abst)) {}

void Tree::SetMaxRaises(int max_raises) {
  this->max_raises_ = max_raises;
}

void Tree::SetActionAbstraction(std::unique_ptr<ActionAbstraction> abst) {
    abst_ = std::move(abst);
}

void Tree::Build() {
}

Node Tree::GetNode(NodeKey k) {
  if (!this->nodes_.contains(k))
    throw std::runtime_error("key not found in Tree: " + std::to_string(k));
  return this->nodes_[k];
}

NodeKey Tree::GetKey(InfoSet i) {
  if (!this->keys_.contains(i))
    throw std::runtime_error("key not found for InfoSet");
  return this->keys_[i];
}
