#pragma once
#include <unordered_map>
#include "solver/range.h"
#include "action/action_abstraction.h"
#include "info_set/info_set.h"
#include "info_set/info_set_abstraction.h"
#include "tree/node.h"

class Tree {
  private:
    int max_raises_;
    int pot_;
    std::vector<int> starting_stacks_;
    std::vector<Card> flop_;
    std::unordered_map<NodeKey, NodeIdx> idx_;
    std::vector<Node> nodes_;
    std::unique_ptr<ActionAbstraction> action_abst_;
    std::unique_ptr<InfoSetAbstraction> info_set_abst_;
    NodeIdx BuildSubtree(InfoSet i);
    NodeIdx CreateNode(InfoSet i, std::vector<NodeIdx> children);
    NodeIdx CreateKey(InfoSet i);
  public:
    Tree(int pot, int max_raises, std::vector<int> starting_stacks,
        std::unique_ptr<ActionAbstraction> abst,
        std::unique_ptr<InfoSetAbstraction> info_set_abst,
        std::vector<Card> flop);
    int Size();
    void SetMaxRaises(int max_raises);
    void SetActionAbstraction(std::unique_ptr<ActionAbstraction> action_abst);
    void SetFlop(std::vector<Card> flop);
    void Reset();
    void Build();
    NodeIdx GetNodeIdx(InfoSet i);
};
