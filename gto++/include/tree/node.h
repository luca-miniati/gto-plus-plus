#pragma once
#include <memory>
#include <vector>
#include "utils/utils.h"
#include "game/game_state.h"
class Node;
using NodePtr = std::shared_ptr<Node>;
using NodeKey = __uint128_t;
using NodeIdx = std::size_t;

class Node {
  private:
    bool is_terminal_;
    bool is_chance_node_;
    int num_actions_;
    NodeIdx idx_;
    std::vector<NodeIdx> children_;
  public:
    Node(NodeIdx idx, int num_actions, std::vector<NodeIdx> children,
        bool is_chance_node, bool is_terminal);
    bool IsTerminal();
    bool IsChanceNode();
    double GetUtility(Deck deck);
    std::vector<double> GetStrategy(double reach_p);
    void UpdateRegret(int action_idx, double value);
    int GetNumActions();
    std::vector<NodeIdx> GetChildren();
    NodeIdx GetChild(int action_idx);
};
