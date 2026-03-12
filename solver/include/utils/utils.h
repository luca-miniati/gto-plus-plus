#pragma once
#include <cstddef>
#include <vector>
#include <random>
#include <algorithm>
#include <array>
#include "phevaluator/phevaluator.h"
using Card = phevaluator::Card;
using Cards = std::vector<Card>;

constexpr std::array<char, 4> SUITS = {'c','d','h','s'};
constexpr std::array<char, 13> RANKS = {'2','3','4','5','6','7','8','9','T','J','Q','K','A'};

inline void HashCombine(std::size_t& seed, std::size_t value) {
  seed ^= value + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

inline const std::array<Card, 52> CARDS = {
  Card("2c"), Card("2d"), Card("2h"), Card("2s"),
  Card("3c"), Card("3d"), Card("3h"), Card("3s"),
  Card("4c"), Card("4d"), Card("4h"), Card("4s"),
  Card("5c"), Card("5d"), Card("5h"), Card("5s"),
  Card("6c"), Card("6d"), Card("6h"), Card("6s"),
  Card("7c"), Card("7d"), Card("7h"), Card("7s"),
  Card("8c"), Card("8d"), Card("8h"), Card("8s"),
  Card("9c"), Card("9d"), Card("9h"), Card("9s"),
  Card("Tc"), Card("Td"), Card("Th"), Card("Ts"),
  Card("Jc"), Card("Jd"), Card("Jh"), Card("Js"),
  Card("Qc"), Card("Qd"), Card("Qh"), Card("Qs"),
  Card("Kc"), Card("Kd"), Card("Kh"), Card("Ks"),
  Card("Ac"), Card("Ad"), Card("Ah"), Card("As")
};

inline Cards MakeDeck() {
  Cards deck(52);;
  int n = SUITS.size();
  int m = RANKS.size();
  for (int i = 0; i < n; ++i)
    for (int j = 0; j < m; ++j)
      deck[i * m + j] = Card(
          std::string(1, RANKS[j]) +
          std::string(1, SUITS[i])
         );
  return deck;
}

inline void ShuffleDeck(Cards &deck) {
  static std::random_device rd;
  static std::mt19937 gen {rd()};
  std::ranges::shuffle(deck, gen);
}

inline bool Contains(const Cards& cards, const Card& card) {
  return std::ranges::find(cards, card) != cards.end();
}