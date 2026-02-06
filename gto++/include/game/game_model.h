#pragma once

#include "game/game_state.h"
#include "action/action.h"

/*
 * The main purpose of the GameModel is to help build the game tree.
 */
class GameModel {
  public:
    GameState Step(const GameState& state, const Action& action);
    bool IsLegal(const GameState& state, const Action& action); 
};
