#include <stdexcept>
#include "game/game_state.h"
#include "utils/utils.h"

GameState GameState::InitialState(int pot, std::vector<int> starting_stacks, Deck deck) {
  if (pot <= 0)
    throw std::runtime_error("initial pot size must be a positive integer");

  std::vector<Card> flop = {deck.Pop(), deck.Pop(), deck.Pop()};

  return GameState(
    /*is_terminal         =*/ false,
    /*current_player      =*/ 0,
    /*current_raises      =*/ 0,
    /*pot                 =*/ pot,
    /*street              =*/ Street::Flop,
    /*deck                =*/ deck,
    /*community_cards     =*/ flop,
    /*current_stacks      =*/ starting_stacks,
    /*current_bets        =*/ {0, 0},
    /*history             =*/ {}
  );
}

bool GameState::operator==(const GameState& other) const {
  return this->current_player == other.current_player &&
    this->current_raises      == other.current_raises &&
    this->pot                 == other.pot &&
    this->street              == other.street &&
    this->community_cards     == other.community_cards &&
    this->current_stacks      == other.current_stacks &&
    this->current_bets        == other.current_bets &&
    this->history             == other.history;
}

std::size_t std::hash<GameState>::operator()(const GameState& s) const {
  std::size_t seed = 0;
  std::hash<int> hasher;
  hash_combine(seed, hasher(s.current_player));
  hash_combine(seed, hasher(s.current_raises));
  hash_combine(seed, hasher(s.pot));
  hash_combine(seed, hasher(static_cast<int>(s.street)));
  for (Card c : s.community_cards)
    hash_combine(seed, hasher(int(c)));
  for (int x : s.current_stacks)
    hash_combine(seed, hasher(x));
  for (int x : s.current_bets)
    hash_combine(seed, hasher(x));
  for (Action a : s.history)
    hash_combine(seed, std::hash<Action>{}(a));
  return seed;
}
