#include "action/per_street_abstraction.h"

std::vector<Action> PerStreetAbstraction::GetActions(const GameState& state) const {
  switch (state.street) {
    case Street::Flop:
      return this->flop_abst_.GetActions(state);
    case Street::Turn:
      return this->turn_abst_.GetActions(state);
    case Street::River:
      return this->river_abst_.GetActions(state);
  }
  return {};
}
