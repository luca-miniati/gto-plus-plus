#include <algorithm>
#include <random>
#include <string>
#include "utils/utils.h"

Deck::Deck() {
  this->deck_.resize(52);

  const std::string suits = "cdhs";
  const std::string ranks = "23456789TJQKA";

  int n = suits.size();
  int m = ranks.size();
  for (int i = 0; i < n; ++i)
    for (int j = 0; j < m; ++j)
      this->deck_[i * m + j] = Card(
          std::string(1, ranks[j]) +
          std::string(1, suits[i])
         );
}

void Deck::Shuffle() {
  std::random_device rd;
  std::mt19937 gen {rd()};
  std::ranges::shuffle(this->deck_, gen);
}

Card Deck::Pop() {
  Card c = this->deck_.back();
  this->deck_.pop_back();
  return c;
}
