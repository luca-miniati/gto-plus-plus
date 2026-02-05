#pragma once

#include <vector>
#include "action.h"
#include "phevaluator/card.h"

class Range {
  private:
  public:
    [[nodiscard]] std::vector<Action> GetActions() const;

    phevaluator::Card()
};
