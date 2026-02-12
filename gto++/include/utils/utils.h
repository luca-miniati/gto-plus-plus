#pragma once
#include <cstddef>
#include <vector>
#include "phevaluator/phevaluator.h"
using Card = phevaluator::Card;

constexpr std::string SUITS = "cdhs";
constexpr std::string RANKS = "23456789TJQKA";

inline void hash_combine(std::size_t& seed, std::size_t value) {
    seed ^= value + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

class Deck {
  private:
    std::vector<Card> deck_;
  public:
    Deck();
    int Size();
    void Shuffle();
    Card Pop();
    void DeleteCard(Card card);
    void AddCard(Card card);
    std::vector<Card> GetCards();
    std::vector<std::vector<Card>> GetCombos(int n);
};

inline const std::vector<Card> CARDS = {
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
