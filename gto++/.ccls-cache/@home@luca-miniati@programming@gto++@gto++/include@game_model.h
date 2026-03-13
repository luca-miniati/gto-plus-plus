#pragma once

#include "game_state.h"
#include "action.h"

class GameModel {
  public:
    GameState step(const GameState &state, const Action &action);
};
