#pragma once

#include "game/game_state.h"
#include "phevaluator/phevaluator.h"
using Card = phevaluator::Card;

struct InfoSet {
  GameState state;
  std::vector<Card> hole_cards;
  bool operator==(const InfoSet& other) const;
};

template<>
struct std::hash<InfoSet> {
  std::size_t operator()(const InfoSet& i) const;
};
