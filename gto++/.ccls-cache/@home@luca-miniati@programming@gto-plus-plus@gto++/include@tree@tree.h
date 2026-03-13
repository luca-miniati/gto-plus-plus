#pragma once
#include <unordered_map>
#include "solver/range.h"
#include "action/action_abstraction.h"
#include "info_set/info_set.h"
#include "info_set/info_set_abstraction.h"
#include "tree/node.h"

class Tree {
  public:
    /* 
     * Construct a tree rooted at `initial_state`. `initial_state` may
     * represent any point in the game (preflop, flop, turn, or river). The
     * caller is responsible for ensuring that `initial_state` is consistent.
     */
    Tree(GameState initial_state,
        int max_raises,
        std::unique_ptr<ActionAbstraction> action_abst,
        std::unique_ptr<InfoSetAbstraction> info_set_abst);

    /*
     * Build (or rebuild) the tree from the initial state.
     * Clears any previously built data.
     */
    void Build();

    std::size_t Size() const                  { return nodes_.size(); }
    NodeIdx     Root() const                  { return root_idx_; }
    bool        IsTerminal(NodeIdx i) const   { return nodes_[i].IsTerminal(); }
    bool        IsChance(NodeIdx i) const     { return nodes_[i].IsChance();   }
    int         NumChildren(NodeIdx i) const  { return nodes_[i].num_children; }

    /*
     * Return the NodeIdx of the i-th child of u (0-indexed)
     */
    NodeIdx Child(NodeIdx u, int i) const {
      return edges_[nodes_[u].fst_child + i];
    }

  private:
    /*
     * Configuration
     */
    GameState initial_state_;
    int max_raises_;
    std::unique_ptr<ActionAbstraction> action_abst_;
    std::unique_ptr<InfoSetAbstraction> info_set_abst_;

    /*
     * Tree data (populated by Build())
     */
    NodeIdx root_idx_;
    std::vector<Node> nodes_;
    std::vector<NodeIdx> edges_;

    /*
     * Allocate a node for `state` and append it to nodes_.
     * Returns the new node's index.
     */
    NodeIdx AllocNode(const GameState &state);
    void BuildIterative();
    TerminalIdx InitTerminalUtilityMatrix(const GameState &state);

    PublicInfoKey GetPublicInfoKey(
        const Cards &community_cards,
        const std::vector<int> &history) const;
    PrivateInfoKey GetPrivateInfoKey(
        const Cards &community_cards,
        const Cards &hole_cards) const;
};
