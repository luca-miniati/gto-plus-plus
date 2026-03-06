#pragma once
#include <functional>
#include <vector>
#include <stdexcept>
#include "action/action.h"
#include "utils/utils.h"
#include "phevaluator/phevaluator.h"
using Chips = int;

enum class Street { Preflop, Flop, Turn, River };

inline Street NextStreet(Street s) {
  switch (s) {
    case Street::Preflop:
      return Street::Flop;
    case Street::Flop:
      return Street::Turn;
    case Street::Turn:
      return Street::River;
    default:
      throw std::runtime_error("can't call NextStreet on Street::River");
  }
}

struct GameState {
  bool is_terminal;
  int current_player;
  int current_raises;
  Street street;
  Cards community_cards;
  std::vector<Chips> current_stacks;
  std::vector<Chips> current_bets;
  std::vector<Chips> pot_contributions;
  std::vector<int> history;
  bool operator==(const GameState& other) const;
  bool IsChance() const;
  static GameState InitialState(std::vector<Chips> pot_contributions,
      std::vector<Chips> starting_stacks, Cards flop);
};

template<>
struct std::hash<GameState> {
  std::size_t operator()(const GameState& other) const;
};
