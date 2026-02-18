#pragma once
#include <unordered_map>
#include "solver/range.h"
#include "action/action_abstraction.h"
#include "info_set/info_set.h"
#include "info_set/info_set_abstraction.h"
#include "tree/node.h"

class Tree {
  private:
    int max_raises_;
    std::vector<Chips> pot_contributions_;
    std::vector<Chips> starting_stacks_;
    Cards flop_;

    NodeIdx root_idx_;
    std::unordered_map<PublicInfoKey, NodeIdx> idx_;
    std::vector<Node> nodes_;
    std::vector<NodeIdx> edges_;

    std::unique_ptr<ActionAbstraction> action_abst_;
    std::unique_ptr<InfoSetAbstraction> info_set_abst_;

    NodeIdx BuildSubtree(
        const GameState &state,
        NodeMap &node_map
        );
    NodeIdx CreateNode(
        const PublicInfoKey &key,
        const GameState &state
        );
    void BuildEdges(
        NodeIdx idx,
        NodeMap &node_map
        );
  public:
    Tree(std::vector<Chips> pot_contributions, int max_raises,
        std::vector<Chips> starting_stacks,
        std::unique_ptr<ActionAbstraction> abst,
        std::unique_ptr<InfoSetAbstraction> info_set_abst,
        Cards flop);

    std::size_t Size() const;

    void Build();

    bool HasNode(PublicInfoKey key) const;
    NodeIdx Child(NodeIdx u, NodeIdx child_idx) const;
    NodeIdx Root() const;
    int NumChildren(NodeIdx u) const;
    bool IsTerminal(NodeIdx u) const;

    const PublicInfoKey GetPublicInfoKey(const Cards &community_cards,
        const std::vector<int> &history) const;
    const PrivateInfoKey GetPrivateInfoKey(const Cards &community_cards,
        const Cards &hole_cards) const;
};
