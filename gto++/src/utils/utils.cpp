#include <algorithm>
#include <random>
#include <string>
#include <stdexcept>
#include "utils/utils.h"

Deck::Deck() {
  this->deck_.resize(52);

  int n = SUITS.size();
  int m = RANKS.size();
  for (int i = 0; i < n; ++i)
    for (int j = 0; j < m; ++j)
      this->deck_[i * m + j] = Card(
          std::string(1, RANKS[j]) +
          std::string(1, SUITS[i])
         );
}

int Deck::Size() {
  return this->deck_.size();
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

void Deck::DeleteCard(Card card) {
  for (auto it = this->deck_.begin(); it != this->deck_.end(); ++it) {
    if (*it == card) {
      this->deck_.erase(it);
      return;
    }
  }
  throw std::runtime_error("card " + card.describeCard() + " not found in deck");
}

void Deck::AddCard(Card card) {
  this->deck_.push_back(card);
}

std::vector<Card> Deck::GetCards() {
  return this->deck_;
}

std::vector<std::vector<Card>> Deck::GetCombos(int n) {
  std::vector<std::vector<Card>> combos;
  switch (n) {
    case 3:
      for (int f1 = 0;      f1 < this->Size(); ++f1)
      for (int f2 = f1 + 1; f2 < this->Size(); ++f2)
      for (int f3 = f2 + 1; f3 < this->Size(); ++f3)
        combos.push_back({this->deck_[f1], this->deck_[f2], this->deck_[f3]});
      break;
    case 1:
      for (int c = 0; c < this->Size(); ++c)
        combos.push_back({this->deck_[c]});
      break;
    default:
      throw std::runtime_error("invalid n: " + std::to_string(n));
  }
  return combos;
}
