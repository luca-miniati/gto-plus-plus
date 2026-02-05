#pragma once

#include <vector>
#include "action.h"
#include "phevaluator/phevaluator.h"
using Card = phevaluator::Card;

enum class Street { Preflop, Flop, Turn, River };

class GameState {
  private:
    int current_player_;
    int max_raises_;
    int current_raises_;
    double pot_;
    Street street_;
    std::vector<Card> community_cards_;
    std::vector<double> blinds_or_straddles_;
    std::vector<double> starting_stacks_;
    std::vector<double> stack_;
    std::vector<double> current_bets_;
    std::vector<Action> history_;
  public:
    [[nodiscard]] Street GetStreet() const;
};
