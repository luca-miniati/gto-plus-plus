#pragma once

#include <vector>
#include <functional>
#include "action/action.h"
#include "phevaluator/phevaluator.h"
using Card = phevaluator::Card;

enum class Street { Preflop, Flop, Turn, River };

struct GameState {
  int current_player;
  int current_raises;
  int pot;
  Street street;
  std::vector<Card> community_cards;
  std::vector<int> blinds_or_straddles;
  std::vector<int> starting_stacks;
  std::vector<int> current_stacks;
  std::vector<int> current_bets;
  std::vector<Action> history;
  bool operator==(const GameState& other) const;
  static GameState InitialState();
};

template<>
struct std::hash<GameState> {
  std::size_t operator()(const GameState& other) const;
};
