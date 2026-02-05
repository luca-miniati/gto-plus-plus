#pragma once

enum class ActionType { Fold, Call, Raise };

class Action {
private:
  const ActionType type_;
  const double amount_;
public:
  Action(ActionType type, int amount);
  ActionType GetType() const;
  int GetAmount() const;
};
