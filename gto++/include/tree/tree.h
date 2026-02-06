#pragma once
#include <unordered_map>
#include "solver/range.h"
#include "tree/info_set.h"
#include "tree/node.h"
using NodeKey = uint64_t;

class Tree {
private:
  int num_nodes_;
  std::unordered_map<InfoSet, NodeKey> keys_;
  std::unordered_map<NodeKey, Node> nodes_;
public:
  Node GetNode(NodeKey key);
};
