#pragma once

enum class ActionType { Bet, Call, Check, Fold, Raise };

/*
 * An Action is a No Limit Holdem action. For Actions with `type_` in {Call,
 * Check, Fold}, `amount_` is set to -1.0.
 */
class Action {
private:
  const ActionType type_;
  const double amount_;
public:
  explicit Action(ActionType type);
  Action(ActionType type, double amount);
  ActionType GetType() const;
  double GetAmount() const;
};
