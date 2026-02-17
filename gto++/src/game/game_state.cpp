#include <stdexcept>
#include "game/game_state.h"
#include "utils/utils.h"

GameState GameState::InitialState(std::vector<Chips> pot_contributions,
    std::vector<Chips> starting_stacks, Cards flop) {
  return GameState(
    /*is_terminal         =*/ false,
    /*current_player      =*/ 0,
    /*current_raises      =*/ 0,
    /*street              =*/ Street::Flop,
    /*community_cards     =*/ flop,
    /*current_stacks      =*/ starting_stacks,
    /*current_bets        =*/ {0, 0},
    /*pot_contributions   =*/ pot_contributions,
    /*history             =*/ {}
  );
}

bool GameState::operator==(const GameState& other) const {
  return this->current_player == other.current_player &&
    this->current_raises      == other.current_raises &&
    this->street              == other.street &&
    this->community_cards     == other.community_cards &&
    this->current_stacks      == other.current_stacks &&
    this->current_bets        == other.current_bets &&
    this->pot_contributions   == other.pot_contributions &&
    this->history             == other.history;
}

std::size_t std::hash<GameState>::operator()(const GameState& s) const {
  std::size_t seed = 0;
  std::hash<int> hasher;
  HashCombine(seed, hasher(s.current_player));
  HashCombine(seed, hasher(s.current_raises));
  HashCombine(seed, hasher(static_cast<int>(s.street)));
  for (Card c : s.community_cards)
    HashCombine(seed, hasher(int(c)));
  for (Chips x : s.current_stacks)
    HashCombine(seed, hasher(x));
  for (Chips x : s.current_bets)
    HashCombine(seed, hasher(x));
  for (Chips x : s.pot_contributions)
    HashCombine(seed, hasher(x));
  for (int x : s.history)
    HashCombine(seed, hasher(x));
  return seed;
}

bool GameState::IsChanceNode() const {
  switch (this->street) {
    case Street::Flop:
      return this->community_cards.size() < 3;
    case Street::Turn:
      return this->community_cards.size() < 4;
    case Street::River:
      return this->community_cards.size() < 5;
    default:
      throw std::runtime_error("invalid street");
  }
}
