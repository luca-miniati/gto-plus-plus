#pragma once

#include "game_state.h"
#include "phevaluator/phevaluator.h"
using Card = phevaluator::Card;

class InfoSet {
  public:
    InfoSet(const GameState& state , std::vector<Card> hole_cards);
};
