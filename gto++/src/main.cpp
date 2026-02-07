#include <iostream>
#include <string>
#include "utils/utils.h"
#include "phevaluator/phevaluator.h"
using Card = phevaluator::Card;

int main() {
  Deck d;
  d.Shuffle();
  for (int i = 0; i < 52; ++i) {
    Card c = d.Pop();
    std::cout << std::string(c) << " \n"[i == 51];
  }
}
