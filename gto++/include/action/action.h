#pragma once

#include <functional>

enum class ActionType { Bet, Call, Check, Fold, Raise };

/*
 * An Action is a No Limit Holdem action. For Actions with type in {Call,
 * Check, Fold}, amount is in units of chips, and is initialized to -1.
 *
 * For Raise, amount is the cumulative sum of all bets on this street,
 * including the raise. In other words, if I've Bet(2), villain Raise(4)'s,
 * and I Raise(8), then I will have 8 chips in front of me and villain will
 * have 4 chips in front of them.
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
