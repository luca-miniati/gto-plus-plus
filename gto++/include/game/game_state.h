#pragma once

#include <functional>
#include <vector>
#include <stdexcept>
#include "action/action.h"
#include "game/game_state.h"
#include "utils/utils.h"
#include "phevaluator/phevaluator.h"
using Card = phevaluator::Card;

enum class Street { Flop, Turn, River };

inline Street NextStreet(Street s) {
  switch (s) {
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
  int pot;
  Street street;
  Deck deck;
  std::vector<Card> community_cards;
  std::vector<int> current_stacks;
  std::vector<int> current_bets;
  std::vector<Action> history;
  bool operator==(const GameState& other) const;
  static GameState InitialState(int pot, std::vector<int> starting_stacks, Deck deck);
};

template<>
struct std::hash<GameState> {
  std::size_t operator()(const GameState& other) const;
};
