#include <stdexcept>
#include <algorithm>
#include <iostream>
#include <cassert>
#include <format>
#include "tree/tree.h"
#include "game/game_model.h"

Tree::Tree(std::vector<Chips> pot_contributions, int max_raises,
    std::vector<Chips> starting_stacks,
    std::unique_ptr<ActionAbstraction> action_abst,
    std::unique_ptr<InfoSetAbstraction> info_set_abst,
    Cards flop)
  : pot_contributions_(pot_contributions),
    max_raises_(max_raises), 
    starting_stacks_(starting_stacks),
    action_abst_(std::move(action_abst)),
    info_set_abst_(std::move(info_set_abst)),
    flop_(flop) {}

void Tree::SetMaxRaises(int max_raises) {
  max_raises_ = max_raises;
}

void Tree::SetActionAbstraction(std::unique_ptr<ActionAbstraction> action_abst) {
  action_abst_ = std::move(action_abst);
}

void Tree::SetFlop(std::vector<Card> flop) {
  flop_ = flop;
}

const PublicInfoKey Tree::GetPublicInfoKey(const Cards &community_cards,
    const std::vector<int> &history) const {
  return info_set_abst_->GetPublicInfoKey(community_cards, history);
}
const PrivateInfoKey Tree::GetPrivateInfoKey(const Cards &community_cards,
        const Cards &hole_cards) const {
  return info_set_abst_->GetPrivateInfoKey(community_cards, hole_cards);
}

NodeIdx Tree::CreateNode(GameState state, std::vector<NodeIdx> children) {
  PublicInfoKey key = GetPublicInfoKey(state.community_cards, state.history);
  if (idx_.contains(key))
    return idx_[key];

  NodeIdx idx = nodes_.size();
  idx_[key] = idx;

  std::optional<GameState> terminal_game_state = state.is_terminal ? std::optional<GameState>{state} : std::nullopt;

  if terminal:
    x = info_set_abst_->GetAl

  bool is_chance_node = state.IsChanceNode();
  NodePtr u = std::make_shared<Node>(
      Node(is_chance_node,
        state.current_player,
        children.size(),
        terminal_game_state,
        children)
      );
  nodes_.push_back(u);
  return idx;
}

NodeIdx Tree::BuildSubtree(GameState state) {
  if (state.is_terminal)
    return CreateNode(state, {});

  std::vector<NodeIdx> children;
  if (state.IsChanceNode()) {  // chance node: we need to deal the turn/river
    for (Card c : CARDS) {
      if (!std::ranges::contains(state.community_cards, c)) {
        GameState next_state = state;
        next_state.community_cards.push_back(c);
        children.push_back(BuildSubtree(next_state));
      }
    }
  } else {  // player node: player makes an action
    int mn_unused_action = 0;
    auto actions = action_abst_->GetActions(state);
    for (int action_idx = 0; action_idx < actions.size(); ++action_idx) {
      if (GameModel::IsLegal(state, actions[action_idx], max_raises_)) {
        GameState next_state = GameModel::Step(state, actions[action_idx],
            mn_unused_action++);
        children.push_back(BuildSubtree(next_state));
      }
    }
  }
  return CreateNode(state, children);
}

void Tree::Build() {
  idx_.clear();
  nodes_.clear();

  GameState state = GameState::InitialState(pot_contributions_,
      starting_stacks_, flop_);
  root_idx_ = BuildSubtree(state);
}

int Tree::Size() {
  return nodes_.size();
}

NodePtr Tree::GetNode(const PublicInfoKey &key) {
  return nodes_[idx_.at(key)];
}

NodePtr Tree::GetChild(const NodePtr &u, int child_idx) {
  NodeIdx idx = u->GetChildIdx(child_idx);
  return nodes_[idx];
}

NodePtr Tree::GetRoot() {
  return nodes_[root_idx_];
}
