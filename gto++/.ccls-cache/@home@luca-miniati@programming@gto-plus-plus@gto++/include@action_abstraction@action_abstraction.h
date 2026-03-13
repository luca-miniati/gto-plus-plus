#pragma once

#include <vector>

class Action;
class GameState;

/*
 * Given a GameState, an ActionAbstraction defines a set of actions that the
 * current player can take.
 */
class ActionAbstraction {
public:
  virtual ~ActionAbstraction() = default;

  /*
   * Return a vector of Actions that the current player can take.
   */
  virtual std::vector<Action> GetActions(const GameState& state) const = 0;
};
