#include "action/check_fold_abstraction.h"

std::vector<Action> CheckFoldAbstraction::GetActions(const GameState &state) const {
  Chips b0 = state.current_bets[0];
  Chips b1 = state.current_bets[1];

  if (b0 == b1)
    return {Action(ActionType::Check)};
  else
    return {Action(ActionType::Fold)};
}
