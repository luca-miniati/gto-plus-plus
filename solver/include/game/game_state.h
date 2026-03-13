#pragma once
#include <functional>
#include <vector>
#include <stdexcept>
#include <array>
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
  std::array<Chips, 2> current_stacks;
  std::array<Chips, 2> current_bets;
  std::array<Chips, 2> pot_contributions;
  std::vector<int> history;
  bool operator==(const GameState& other) const;
  bool IsChance() const;
  GameState(bool is_terminal,
          int current_player,
          int current_raises,
          Street street,
          Cards community_cards,
          std::array<Chips, 2> current_stacks,
          std::array<Chips, 2> current_bets,
          std::array<Chips, 2> pot_contributions,
          std::vector<int> history)
    : is_terminal(is_terminal),
      current_player(current_player),
      current_raises(current_raises),
      street(street),
      community_cards(community_cards),
      current_stacks(std::move(current_stacks)),
      current_bets(std::move(current_bets)),
      pot_contributions(std::move(pot_contributions)),
      history(std::move(history)) {}
  static GameState InitialState(std::array<Chips, 2> pot_contributions,
      std::array<Chips, 2> starting_stacks, Cards flop);
};

template<>
struct std::hash<GameState> {
  std::size_t operator()(const GameState& other) const;
};
