#pragma once
#include <unordered_map>
#include "solver/range.h"
#include "action/action_abstraction.h"
#include "info_set/info_set.h"
#include "info_set/info_set_abstraction.h"
#include "tree/node.h"

class Tree {
  private:
    int root_idx_;
    int max_raises_;
    std::vector<Chips> pot_contributions_;
    std::vector<Chips> starting_stacks_;
    Cards flop_;
    std::unordered_map<PublicInfoKey, NodeIdx> idx_;
    std::vector<NodePtr> nodes_;
    std::unique_ptr<ActionAbstraction> action_abst_;
    std::unique_ptr<InfoSetAbstraction> info_set_abst_;
    NodeIdx BuildSubtree(GameState state);
    NodeIdx CreateNode(GameState state, std::vector<NodeIdx> children);
  public:
    Tree(std::vector<Chips> pot_contributions, int max_raises,
        std::vector<Chips> starting_stacks,
        std::unique_ptr<ActionAbstraction> abst,
        std::unique_ptr<InfoSetAbstraction> info_set_abst,
        Cards flop);
    int Size();
    void SetMaxRaises(int max_raises);
    void SetActionAbstraction(std::unique_ptr<ActionAbstraction> action_abst);
    void SetFlop(Cards flop);
    void Reset();
    void Build();
    NodePtr GetNode(const PublicInfoKey &key);
    NodePtr GetChild(const NodePtr &u, int child_idx);
    NodePtr GetRoot();
    const PublicInfoKey GetPublicInfoKey(const Cards &community_cards,
        const std::vector<int> &history) const;
    const PrivateInfoKey GetPrivateInfoKey(const Cards &community_cards,
        const Cards &hole_cards) const;
};
