#pragma once

#include "game_state.h"
#include "phevaluator/phevaluator.h"
using Card = phevaluator::Card;

class InfoSet {
  private:
    GameState state_;
    std::vector<Card> hole_cards_;
  public:
    InfoSet(GameState state, std::vector<Card> hole_cards);

    const GameState GetState() const;

    const std::vector<Card> GetHoleCards() const;
};
