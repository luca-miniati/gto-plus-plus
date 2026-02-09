#pragma once
#include <memory>
#include <vector>
#include "utils/utils.h"
#include "game/game_state.h"
class Node;
using NodePtr = std::shared_ptr<Node>;
using NodeKey = std::size_t;

class Node {
  private:
    bool is_terminal_;
    bool is_chance_node_;
    int num_actions_;
    NodeKey key_;
    std::vector<NodePtr> children_;
  public:
    Node(NodeKey key, int num_actions, std::vector<NodePtr> children,
        bool is_chance_node, bool is_terminal);
    NodeKey GetKey();
    bool IsTerminal();
    bool IsChanceNode();
    double GetUtility(Deck deck);
    std::vector<double> GetStrategy(double reach_p);
    void UpdateRegret(int action_idx, double value);
    int GetNumActions();
    std::vector<NodePtr> GetChildren();
    NodePtr GetChild(int action_idx);
};
