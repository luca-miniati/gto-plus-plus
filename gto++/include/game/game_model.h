#pragma once

#include "game/game_state.h"
#include "action/action.h"

/*
 * The main purpose of the GameModel is to help build the game tree.
 */
class GameModel {
  public:
    static bool IsLegal(const GameState &state, const Action &action); 
    static GameState Step(const GameState &state, const Action &action);
};
