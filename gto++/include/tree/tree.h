#pragma once
#include <unordered_map>
#include "solver/range.h"
#include "action/action_abstraction.h"
#include "tree/info_set.h"
#include "tree/node.h"

class Tree {
  private:
    int max_raises_;
    int pot_;
    std::vector<int> starting_stacks_;
    std::vector<Card> flop_;
    std::unordered_map<InfoSet, NodeKey> keys_;
    std::vector<Node> nodes_;
    std::unique_ptr<ActionAbstraction> abst_;
    NodePtr BuildSubtree(InfoSet i);
    NodePtr CreateNode(InfoSet i, std::vector<NodePtr> children);
    NodeKey CreateKey(InfoSet i);
  public:
    Tree(int pot, int max_raises, std::vector<int> starting_stacks,
        std::unique_ptr<ActionAbstraction> abst, std::vector<Card> flop);
    int Size();
    void SetMaxRaises(int max_raises);
    void SetActionAbstraction(std::unique_ptr<ActionAbstraction> abst);
    void SetFlop(std::vector<Card> flop);
    void Reset();
    void Build();
    NodePtr GetNode(InfoSet i);

    std::unordered_map<InfoSet, NodeKey> GetKeys();
};
