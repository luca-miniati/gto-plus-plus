#pragma once

#include <functional>

enum class ActionType { Bet, Call, Check, Fold, Raise };

/*
 * An Action is a No Limit Holdem action. For Actions with `type` in {Call,
 * Check, Fold}, `amount` is in units of chips, and is initialized to -1.
 */
struct Action {
  const ActionType type;
  int amount = -1;
  bool operator==(const Action& other) const;
};

template<>
struct std::hash<Action> {
  std::size_t operator()(const Action& a) const;
};
