#include <math.h>
#include "tree/node.h"

Node::Node(bool is_chance_node,
    int current_player,
    std::optional<GameState> terminal_game_state)
  : is_chance_node_(is_chance_node),
    current_player_(current_player),
    terminal_game_state_(terminal_game_state),
    num_children_(0)
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

int Node::GetNumChildren() {
  return num_children_;
}

bool Node::IsChanceNode() {
  return is_chance_node_;
}

bool Node::IsTerminal() {
  return terminal_game_state_.has_value();
}

bool Node::HasStrategy(const PrivateInfoKey &key) const {
  return strategies_.contains(key);
}

void Node::InitStrategy(const PrivateInfoKey &key) {
  std::vector<double> regret_sum(num_children_);
  std::vector<double> strategy(num_children_);
  std::vector<double> strategy_sum(num_children_);
  // init uniform strategy
  for (int action_idx = 0; action_idx < num_children_; ++action_idx)
    strategy[action_idx] = 1.0 / num_children_;

  strategies_[key] = CFRData(regret_sum, strategy, strategy_sum);
}

void Node::UpdateStrategy(PrivateInfoKey key, double reach_p) {
  double norm = 0;
  for (int action_idx = 0; action_idx < num_children_; action_idx++) {
    double value = fmax(GetRegretSum(key, action_idx), 0.0);
    SetStrategy(key, action_idx, value);
    norm += value;
  }
  for (int action_idx = 0; action_idx < num_children_; action_idx++) {
    if (norm > 0)
      NormalizeStrategy(key, action_idx, norm);
    else
      SetStrategy(key, action_idx, 1.0 / num_children_);
    UpdateStrategySum(key, action_idx, reach_p * GetStrategy(key, action_idx));
  }
}

double Node::GetStrategySum(PrivateInfoKey key, int action_idx) const {
  return strategies_.at(key).strategy_sum[action_idx];
}

std::vector<double> Node::GetAverageStrategy(PrivateInfoKey key) const {
  std::vector<double> average_strategy(num_children_);
  double norm = 0;
  for (int action_idx = 0; action_idx < num_children_; action_idx++) {
    double value = GetStrategySum(key, action_idx);
    average_strategy[action_idx] = value;
    norm += value;
  }
  for (int action_idx = 0; action_idx < num_children_; action_idx++) {
    if (norm > 0)
      average_strategy[action_idx] /= norm;
    else
      average_strategy[action_idx] = 1.0 / num_children_;
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

void Node::AddChild(NodeIdx idx) {
  children_.push_back(idx);
  num_children_ = children_.size();
}
