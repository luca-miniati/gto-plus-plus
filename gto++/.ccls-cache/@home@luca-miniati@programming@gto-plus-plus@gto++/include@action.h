#pragma once

#include <functional>

enum class ActionType { Bet = 0, Call, Check, Fold, Raise };

/*
 * An Action is a No Limit Holdem action. For Actions with `type_` in {Call,
 * Check, Fold}, `amount_` is set to -1.
 */
class Action {
  private:
    const ActionType type_;
    const int amount_;
  public:
    explicit Action(ActionType type);
    Action(ActionType type, int amount);
    ActionType GetType() const;
    int GetAmount() const;
};
