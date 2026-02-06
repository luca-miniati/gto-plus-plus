#pragma once

#include <vector>
#include "action/action.h"
#include "action/action_abstraction.h"
#include "game/game_state.h"

/*
 * A PerStreetAbstraction has 3 separate ActionAbstractions for each postflop
 * street: Flop, Turn, and River.
 */
class PerStreetAbstraction : public ActionAbstraction {
  private:
    const ActionAbstraction& flop_abst_;
    const ActionAbstraction& turn_abst_;
    const ActionAbstraction& river_abst_;
  public:
    PerStreetAbstraction(
        const ActionAbstraction& flop_abst, 
        const ActionAbstraction& turn_abst,
        const ActionAbstraction& river_abst
        );

    virtual std::vector<Action> GetActions(const GameState& state) const override;
};
