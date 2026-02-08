#pragma once
#include <memory>
#include <vector>
class Node;
using NodePtr = std::shared_ptr<Node>;

class Node {
  private:
    bool is_terminal_;
    int num_actions_;
    std::vector<double> regret_sum, strategy, strategy_sum;
  public:
    bool IsTerminal();
    std::vector<NodePtr> GetChildren();
};
