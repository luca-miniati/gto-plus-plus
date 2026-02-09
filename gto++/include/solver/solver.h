#pragma once
#include "game/game_state.h"
#include "solver/range.h"
#include "tree/tree.h"
#include "tree/info_set.h"

class Solver {
  private:
    Tree tree;
    double CFR(GameState state, Deck deck, double p1, double p2);
  public:
    void Solve();
    const Range GetStrategy(GameState state) const;
};
