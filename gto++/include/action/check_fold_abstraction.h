#pragma once
#include <vector>
#include "action/action.h"
#include "action/action_abstraction.h"
#include "game/game_state.h"

/*
 * A CheckFoldAbstraction checks if there's no bet, and folds if there's a bet.
 */
class CheckFoldAbstraction : public ActionAbstraction {
private:
public:
  virtual std::vector<Action> GetActions(const GameState& state) const override;
};
