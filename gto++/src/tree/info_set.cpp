#include "utils/utils.h"
#include "tree/info_set.h"
#include "game/game_state.h"

bool InfoSet::operator==(const InfoSet& other) const {
  return this->state == other.state && this->hole_cards == other.hole_cards;
}

std::size_t std::hash<InfoSet>::operator()(const InfoSet& i) const {
  size_t seed = 0;
  hash_combine(seed, std::hash<GameState>{}(i.state));
  for (Card c : i.hole_cards)
    hash_combine(seed, std::hash<int>{}(int(c)));
  return seed;
}
