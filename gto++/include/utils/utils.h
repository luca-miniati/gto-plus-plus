#pragma once
#include <cstddef>
#include <vector>
#include "phevaluator/phevaluator.h"
using Card = phevaluator::Card;

inline void hash_combine(std::size_t& seed, std::size_t value) {
    seed ^= value + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

class Deck {
  private:
    std::vector<Card> deck_;
  public:
    Deck();
    void Shuffle();
    Card Pop();
};
