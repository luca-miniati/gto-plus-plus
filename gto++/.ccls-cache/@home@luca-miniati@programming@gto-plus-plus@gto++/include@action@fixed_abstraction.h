#pragma once
#include <vector>
#include "action/action.h"
#include "action/action_abstraction.h"
#include "game/game_state.h"

/*
 * A FixedAbstractions contains a fixed set of Actions.
 */
class FixedAbstraction : public ActionAbstraction {
private:
  std::vector<Action> actions_;
public:
  explicit FixedAbstraction(std::vector<Action> actions);

  virtual std::vector<Action> GetActions(const GameState& state) const override;
};
