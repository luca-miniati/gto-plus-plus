#pragma once

#include "game/game_state.h"
#include "action/action.h"

/*
 * The main purpose of the GameModel is to help build the game tree.
 * It also performs evaluation of hands, to get the utility of terminal
 * nodes.
 */
class GameModel {
  public:
    static bool IsLegal(const GameState &state, const Action &action,
        int max_raises); 
    static GameState Step(const GameState &state, const Action &action,
        const int action_idx);
    static double GetUtility(const GameState &state,
        const Cards &community_cards, const Cards &p0_cards,
        const Cards &p1_cards);
};
