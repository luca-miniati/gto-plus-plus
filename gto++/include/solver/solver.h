#pragma once
#include "game/game_state.h"
#include "solver/range.h"
#include "tree/info_set.h"

class Solver {
  private:
  public:
    void solve();

    const Range GetStrategy(GameState state) const;
};
