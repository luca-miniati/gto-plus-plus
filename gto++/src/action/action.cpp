#include "action/action.h"
#include "utils/utils.h"

bool Action::operator==(const Action& other) const {
  return this->type == other.type && this->amount == other.amount;
}

std::size_t std::hash<Action>::operator()(const Action& a) const {
  std::size_t seed = 0;
  std::hash<int> hasher;
  hash_combine(seed, hasher(static_cast<int>(a.type)));
  hash_combine(seed, hasher(a.amount));
  return seed;
};
