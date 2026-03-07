#pragma once
#include "game/game_state.h"
#include "solver/range.h"
#include "tree/tree.h"
#include "info_set/info_set.h"

class Solver {
  private:
    // Tree tree_;
    Cards flop_;
    double CFR(NodePtr u,
        std::vector<int> history,
        Cards community_cards,
        Cards deck,
        Cards p0_cards,
        Cards p1_cards,
        double p0,
        double p1);
  public:
    Solver(std::vector<Chips> pot_contributions,
        int max_raises,
        std::vector<Chips> starting_stacks,
        std::unique_ptr<ActionAbstraction> action_abst,
        std::unique_ptr<InfoSetAbstraction> info_set_abst,
        Cards flop);
    void Solve();
    const Range GetStrategy(GameState state) const;
};
