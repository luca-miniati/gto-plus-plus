#include "action/action.h"
#include "utils/utils.h"

Action::Action(ActionType type) : type(type), amount(-1) {}

Action::Action(ActionType type, int amount) : type(type), amount(amount) {}

bool Action::operator==(const Action& other) const {
  return this->type == other.type && this->amount == other.amount;
}

std::size_t std::hash<Action>::operator()(const Action& a) const {
  std::size_t seed = 0;
  std::hash<int> hasher;
  HashCombine(seed, hasher(static_cast<int>(a.type)));
  HashCombine(seed, hasher(a.amount));
  return seed;
};
