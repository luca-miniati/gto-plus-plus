#include <math.h>
#include "tree/node.h"

Node::Node(bool is_chance_node,
    int current_player,
    int num_actions,
    std::optional<GameState> terminal_game_state,
    std::vector<NodeIdx> children)
  : current_player_(current_player),
    num_actions_(num_actions),
    children_(children),
    is_chance_node_(is_chance_node),
    terminal_game_state_(terminal_game_state)
{}

NodeIdx Node::GetChildIdx(int action_idx) {
  return children_[action_idx];
}

std::vector<NodeIdx> Node::GetChildren() {
  return children_;
}

int Node::GetCurrentPlayer() {
  return current_player_;
}

int Node::GetNumActions() {
  return num_actions_;
}

bool Node::IsChanceNode() {
  return is_chance_node_;
}

bool Node::IsTerminal() {
  return terminal_game_state_.has_value();
}

bool Node::HasStrategy(PrivateInfoKey key) {
  return strategies_.contains(key);
}

void Node::InitStrategy(PrivateInfoKey key) {
  std::vector<double> regret_sum(num_actions_);
  std::vector<double> strategy(num_actions_);
  std::vector<double> strategy_sum(num_actions_);
  // init uniform strategy
  for (int action_idx = 0; action_idx < num_actions_; ++action_idx)
    strategy[action_idx] = 1.0 / num_actions_;

  strategies_[key] = CFRData(regret_sum, strategy, strategy_sum);
}

void Node::UpdateStrategy(PrivateInfoKey key, double reach_p) {
  double norm = 0;
  for (int action_idx = 0; action_idx < num_actions_; action_idx++) {
    double value = fmax(GetRegretSum(key, action_idx), 0.0);
    SetStrategy(key, action_idx, value);
    norm += value;
  }
  for (int action_idx = 0; action_idx < num_actions_; action_idx++) {
    if (norm > 0)
      NormalizeStrategy(key, action_idx, norm);
    else
      SetStrategy(key, action_idx, 1.0 / num_actions_);
    UpdateStrategySum(key, action_idx, reach_p * GetStrategy(key, action_idx));
  }
}

// const std::vector<double>& Node::GetStrategy(PrivateInfoKey key) const {
//   return strategies_.at(key).strategy;
// }
//
// const std::vector<double>& Node::GetRegretSum(PrivateInfoKey key) const {
//   return strategies_.at(key).regret_sum;
// }
//
// const std::vector<double>& Node::GetStrategySum(PrivateInfoKey key) const {
//   return strategies_.at(key).strategy_sum;
// }

double Node::GetStrategySum(PrivateInfoKey key, int action_idx) const {
  return strategies_.at(key).strategy_sum[action_idx];
}

std::vector<double> Node::GetAverageStrategy(PrivateInfoKey key) const {
  std::vector<double> average_strategy(num_actions_);
  double norm = 0;
  for (int action_idx = 0; action_idx < num_actions_; action_idx++) {
    double value = GetStrategySum(key, action_idx);
    average_strategy[action_idx] = value;
    norm += value;
  }
  for (int action_idx = 0; action_idx < num_actions_; action_idx++) {
    if (norm > 0)
      average_strategy[action_idx] /= norm;
    else
      average_strategy[action_idx] = 1.0 / num_actions_;
  }
  return average_strategy;
}

double Node::GetRegretSum(PrivateInfoKey key, int action_idx) const {
  return strategies_.at(key).regret_sum[action_idx];
}

double Node::GetStrategy(PrivateInfoKey key, int action_idx) const {
  return strategies_.at(key).strategy[action_idx];
}

void Node::UpdateRegret(PrivateInfoKey key, int action_idx, double value) {
  strategies_.at(key).regret_sum[action_idx] += value;
}

void Node::UpdateStrategySum(PrivateInfoKey key, int action_idx, double value) {
  strategies_.at(key).strategy_sum[action_idx] += value;
}

void Node::SetStrategy(PrivateInfoKey key, int action_idx, double value) {
  strategies_.at(key).strategy[action_idx] = value;
}

void Node::NormalizeStrategy(PrivateInfoKey key, int action_idx, double norm) {
  strategies_.at(key).strategy[action_idx] /= norm;
}

GameState Node::GetTerminalGameState() {
  return terminal_game_state_.value();
}
