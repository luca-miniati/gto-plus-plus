#pragma once

#include <vector>
#include "action.h"

enum class Street { Preflop, Flop, Turn, River };

class GameState {
  private:
    Street street_;
    std::vector<double> blinds_or_straddles_;
    std::vector<Action> history_;
    std::vector<double> starting_stacks_;
    std::vector<double> stack_;
    std::vector<double> current_bets_;
    int current_player_;
    int max_raises_;
    int current_raises_;
    double pot_;
    // std::vector<Card> community_cards_
};
