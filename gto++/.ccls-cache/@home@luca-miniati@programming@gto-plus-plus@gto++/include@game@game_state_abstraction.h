#pragma once
#include "game/game_state.h"

/*
 * Given some GameStates, a GameStateAbstraction maps the set of states to a
 * set of buckets, represented by integers.
 */
class GameStateAbstraction {
  public:
    virtual ~GameStateAbstraction() = default;

    /*
     * Map each GameState to a bucket index.
     */
    virtual int GetBucket(const GameState& state) const = 0;
};
