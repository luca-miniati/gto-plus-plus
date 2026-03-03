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
    flop_(flop)
{
  nodes_.reserve(1'000'000);
  edges_.reserve(1'000'000);
}

const PublicInfoKey Tree::GetPublicInfoKey(const Cards &community_cards,
    const std::vector<int> &history) const {
  return info_set_abst_->GetPublicInfoKey(community_cards, history);
}
const PrivateInfoKey Tree::GetPrivateInfoKey(const Cards &community_cards,
        const Cards &hole_cards) const {
  return info_set_abst_->GetPrivateInfoKey(community_cards, hole_cards);
}

TerminalIdx Tree::InitTerminalUtilityMatrix(const GameState& state) {
}

NodeIdx Tree::CreateNode(
    const PublicInfoKey &key,
    const GameState &state
    ) {
  NodeIdx idx = nodes_.size();
  idx_[key] = idx;

  TerminalIdx terminal_idx = InitTerminalUtilityMatrix(state);

  // init an empty node
  // fields will get populated in BuildEdges
  uint8_t flags = (state.IsChance() ? 2 : 0) | (state.is_terminal ? 1 : 0);
  nodes_.emplace_back(
      /* fst_child                    =*/ SIZE_MAX,
      /* num_children                 =*/ 0,
      /* flags                        =*/ flags,
      /* current_player               =*/ state.current_player,
      /* terminal_utility_matrix_idx  =*/ terminal_idx
      );

  return idx;
}

NodeIdx Tree::BuildSubtree(
    const GameState &state,
    NodeAdjacency &adj
    ) {
  PublicInfoKey key = info_set_abst_->GetPublicInfoKey(
      state.community_cards,
      state.history
      );
  
  assert(!HasNode(key));

  if (state.is_terminal)
    return CreateNode(key, state);

  NodeIdx curr_idx = CreateNode(key, state);
  Node &u = nodes_[curr_idx];

  if (state.IsChance()) {  // chance node: we need to deal the turn/river
    for (Card card : CARDS) {
      if (!std::ranges::contains(state.community_cards, card)) {
        // deal the card
        GameState next_state = GameModel::Step(state, card);

        // lookup canonical key
        PublicInfoKey next_key = info_set_abst_->GetPublicInfoKey(
            next_state.community_cards,
            next_state.history
            );

        // if it's not been built, build subtree
        if (!HasNode(next_key))
          adj[curr_idx].emplace_back(BuildSubtree(next_state, adj));
      }
    }
  } else {  // player node: player makes an action
    auto actions = action_abst_->GetActions(state);
    for (int action_idx = 0; action_idx < actions.size(); ++action_idx) {
      if (GameModel::IsLegal(state, actions[action_idx], max_raises_)) {
        // make the action
        GameState next_state = GameModel::Step(
            /* state      =*/ state,
            /* action     =*/ actions[action_idx],
            /* action_idx =*/ adj[curr_idx].size()
            );

        // lookup canonical key
        PublicInfoKey next_key = info_set_abst_->GetPublicInfoKey(
            next_state.community_cards,
            next_state.history
            );

        // if it's not been built, build subtree
        if (!HasNode(next_key))
          adj[curr_idx].emplace_back(BuildSubtree(next_state, adj));
      }
    }
  }

  return curr_idx;
}

void Tree::BuildEdges(NodeIdx idx, NodeAdjacency &adj) {
  if (nodes_[idx].IsTerminal())
    return;

  // we've build this node
  assert(adj.contains(idx));
  // we haven't built edges yet
  assert(nodes_[idx].fst_child == SIZE_MAX);

  nodes_[idx].fst_child = edges_.size();
  nodes_[idx].num_children = adj[idx].size();

  for (NodeIdx child_idx : adj[idx])
    edges_.emplace_back(child_idx);
  for (NodeIdx child_idx : adj[idx])
    BuildEdges(child_idx, adj);
}

void Tree::Build() {
  idx_.clear();
  nodes_.clear();
  edges_.clear();

  GameState state = GameState::InitialState(pot_contributions_,
      starting_stacks_, flop_);

  NodeAdjacency adj;
  root_idx_ = BuildSubtree(state, adj);
  BuildEdges(root_idx_, adj);
}

std::size_t Tree::Size() const {
  return nodes_.size();
}

bool Tree::HasNode(PublicInfoKey key) const {
  return idx_.contains(key);
}

NodeIdx Tree::Root() const {
  return root_idx_;
}

NodeIdx Tree::Child(NodeIdx u, NodeIdx child_idx) const {
  return edges_[nodes_[u].fst_child + child_idx];
}

int Tree::NumChildren(NodeIdx u) const {
  return nodes_[u].num_children;
}

bool Tree::IsTerminal(NodeIdx u) const {
  return nodes_[u].IsTerminal();
}
