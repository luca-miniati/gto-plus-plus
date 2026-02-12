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

NodeIdx Tree::CreateNode(InfoSet i, std::vector<NodeIdx> children) {
  NodeKey key = this->info_set_abst_->GetKey(i);
  if (this->idx_.contains(key))
    return this->idx_[key];

  NodeIdx idx = this->nodes_.size();
  this->idx_[key] = idx;
  this->nodes_.push_back(
      Node(
        idx,
        children.size(),
        children,
        i.state.IsChanceNode(),
        i.state.is_terminal
        )
      );
  return idx;
}

NodeIdx Tree::GetOrCreateNodeIdx(InfoSet i) {
  NodeKey key = this->info_set_abst_->GetKey(i);
  if (this->idx_.contains(key))
    return this->idx_[key];
  return this->BuildSubtree(i);
}

NodeIdx Tree::BuildSubtree(InfoSet i) {
  if (i.state.is_terminal)
    return this->CreateNode(i, {});

  std::vector<NodeIdx> children;
  if (i.state.IsChanceNode()) {  // chance node: we need to deal the turn/river
    for (Card c : CARDS) {
      if (!std::ranges::contains(i.hole_cards, c) &&
          !std::ranges::contains(i.state.community_cards, c)) {
        InfoSet j{i};
        j.state.community_cards.push_back(c);
        children.push_back(this->BuildSubtree(j));
      }
    }
  } else {  // player node: player makes an action
    auto actions = this->action_abst_->GetActions(i.state);
    for (int action_idx = 0; action_idx < actions.size(); ++action_idx) {
      if (GameModel::IsLegal(i.state, actions[action_idx], this->max_raises_)) {
        GameState next = GameModel::Step(i.state, actions[action_idx], action_idx);
        InfoSet j{next, i.hole_cards};
        children.push_back(this->BuildSubtree(j));
      }
    }
  }
  return this->CreateNode(i, children);
}

void Tree::Build() {
  this->idx_.clear();
  this->nodes_.clear();

  GameState state = GameState::InitialState(this->pot_, this->starting_stacks_, this->flop_);
  for (Card c1 : CARDS)
    for (Card c2 : CARDS)
      if (c1 > c2 &&
          c1 != this->flop_[0] &&
          c1 != this->flop_[1] &&
          c1 != this->flop_[2] &&
          c2 != this->flop_[0] &&
          c2 != this->flop_[1] &&
          c2 != this->flop_[2])
        this->BuildSubtree(InfoSet{state, {c1, c2}});
}

int Tree::Size() {
  return this->nodes_.size();
}
