#include "utils.h"
#include "info_set.h"

InfoSet::InfoSet(GameState state, std::vector<Card> hole_cards)
  : state_(state), hole_cards_(hole_cards) {}

std::size_t std::hash<InfoSet>::operator()(const InfoSet& i) const noexcept {
  std::size_t seed = 0;
  hash_combine(seed, std::hash<GameState>{}(i.GetState()));
  for (Card c : i.GetHoleCards())
    hash_combine(seed, std::hash<int>{}(static_cast<int>(c)));
  return seed;
}
