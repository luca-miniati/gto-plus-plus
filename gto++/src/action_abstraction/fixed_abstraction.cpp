#include <vector>
#include "action_abstraction/fixed_abstraction.h"

FixedAbstraction::FixedAbstraction(std::vector<Action> actions)
  : actions_(actions) {}

std::vector<Action> FixedAbstraction::GetActions(const GameState& state) const {
  return this->actions_;
}
