#pragma once

#include "info_set.h"
#include "range.h"

class Solver {
  private:
  public:
    void solve();

    const Range GetStrategy(GameState state) const;
};
