#include <stdexcept>
#include <algorithm>
#include <iostream>
#include <cassert>
#include "tree/tree.h"
#include "game/game_model.h"

Tree::Tree(int pot, int max_raises, std::vector<int> starting_stacks,
    std::unique_ptr<ActionAbstraction> action_abst,
    std::unique_ptr<InfoSetAbstraction> info_set_abst,
    std::vector<Card> flop)
  : pot_(pot),
    max_raises_(max_raises), 
    starting_stacks_(starting_stacks),
    action_abst_(std::move(action_abst)),
    info_set_abst_(std::move(info_set_abst)),
    flop_(flop) {}

void Tree::SetMaxRaises(int max_raises) {
  this->max_raises_ = max_raises;
}

void Tree::SetActionAbstraction(std::unique_ptr<ActionAbstraction> action_abst) {
  this->action_abst_ = std::move(action_abst);
}

void Tree::SetFlop(std::vector<Card> flop) {
  this->flop_ = flop;
}

NodeIdx Tree::CreateNode(GameState state, std::vector<NodeIdx> children) {
  NodeKey key = this->info_set_abst_->GetPublicKey(state);
  if (this->idx_.contains(key))
    return this->idx_[key];

  NodeIdx idx = this->nodes_.size();
  this->idx_[key] = idx;
  this->nodes_.push_back(
      Node(
        idx,
        children.size(),
        children,
        state.IsChanceNode(),
        state.is_terminal
        )
      );
  return idx;
}

NodeIdx Tree::GetOrCreateNodeIdx(GameState state) {
  NodeKey key = this->info_set_abst_->GetPublicKey(state);
  if (this->idx_.contains(key))
    return this->idx_[key];
  return this->BuildSubtree(state);
}

NodeIdx Tree::BuildSubtree(GameState state) {
  if (state.is_terminal)
    return this->CreateNode(state, {});

  std::vector<NodeIdx> children;
  if (state.IsChanceNode()) {  // chance node: we need to deal the turn/river
    for (Card c : CARDS) {
      if (!std::ranges::contains(state.community_cards, c)) {
        GameState next_state = state;
        next_state.community_cards.push_back(c);
        children.push_back(this->BuildSubtree(next_state));
      }
    }
  } else {  // player node: player makes an action
    auto actions = this->action_abst_->GetActions(state);
    for (int action_idx = 0; action_idx < actions.size(); ++action_idx) {
      if (GameModel::IsLegal(state, actions[action_idx], this->max_raises_)) {
        GameState next_state = GameModel::Step(state, actions[action_idx],
            action_idx);
        children.push_back(this->BuildSubtree(next_state));
      }
    }
  }
  return this->CreateNode(state, children);
}

void Tree::Build() {
  this->idx_.clear();
  this->nodes_.clear();

  GameState state = GameState::InitialState(this->pot_, this->starting_stacks_,
      this->flop_);
  this->BuildSubtree(state);
}

int Tree::Size() {
  return this->nodes_.size();
}
