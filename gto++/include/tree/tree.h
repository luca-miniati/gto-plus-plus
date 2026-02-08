#pragma once
#include <unordered_map>
#include "solver/range.h"
#include "action/action_abstraction.h"
#include "tree/info_set.h"
#include "tree/node.h"
using NodeKey = size_t;

class Tree {
private:
  int num_nodes_;
  int max_raises_;
  std::unordered_map<InfoSet, NodeKey> keys_;
  std::vector<Node> nodes_;
  std::unique_ptr<ActionAbstraction> abst_;
public:
  Tree(int max_raises, std::unique_ptr<ActionAbstraction> abst);
  void SetMaxRaises(int max_raises);
  void SetActionAbstraction(std::unique_ptr<ActionAbstraction> abst);
  void Build();
  NodeKey GetKey(InfoSet i);
  Node GetNode(NodeKey k);
};
