#include <stdexcept>
#include <algorithm>
#include <iostream>
#include <cassert>
#include "tree/tree.h"
#include "game/game_model.h"

Tree::Tree(int pot, int max_raises, std::vector<int> starting_stacks,
    std::unique_ptr<ActionAbstraction> abst, std::vector<Card> flop)
  : pot_(pot),
    max_raises_(max_raises), 
    starting_stacks_(starting_stacks),
    abst_(std::move(abst)),
    flop_(flop) {}

void Tree::SetMaxRaises(int max_raises) {
  this->max_raises_ = max_raises;
}

void Tree::SetActionAbstraction(std::unique_ptr<ActionAbstraction> abst) {
  this->abst_ = std::move(abst);
}

void Tree::SetFlop(std::vector<Card> flop) {
  this->flop_ = flop;
}

NodePtr Tree::CreateNode(InfoSet i, std::vector<NodePtr> children) {
  NodeKey key = this->nodes_.size();
  this->keys_[i] = key;
  this->nodes_.push_back(
      Node(
        key,
        children.size(),
        children,
        i.state.IsChanceNode(),
        i.state.is_terminal
        )
      );
  return std::make_shared<Node>(this->nodes_.back());
}

NodePtr Tree::GetNode(InfoSet i) {
  NodeKey key = this->keys_[i];
  return std::make_shared<Node>(this->nodes_[key]);
}

NodePtr Tree::BuildSubtree(InfoSet i) {
  if (i.state.is_terminal)
    return this->CreateNode(i, {});

  std::vector<NodePtr> children;
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
    auto actions = this->abst_->GetActions(i.state);
    for (int action_idx = 0; action_idx < actions.size(); ++action_idx) {
      if (GameModel::IsLegal(i.state, actions[action_idx], this->max_raises_)) {
        GameState next = GameModel::Step(i.state, actions[action_idx]);
        InfoSet j{next, i.hole_cards};
        children.push_back(this->BuildSubtree(j));
      }
    }
  }
  return this->CreateNode(i, children);
}

void Tree::Build() {
  this->keys_.clear();
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

std::unordered_map<InfoSet, NodeKey> Tree::GetKeys() {return this->keys_;}
