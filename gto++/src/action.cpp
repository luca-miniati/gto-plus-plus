#include "action.h"

Action::Action(ActionType type) : type_(type), amount_(-1) {}

Action::Action(ActionType type, double amount) : type_(type), amount_(amount) {}

ActionType Action::GetType() const {
  return this->type_;
}

double Action::GetAmount() const {
  return this->amount_;
}
