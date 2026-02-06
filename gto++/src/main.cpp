#include <iostream>
#include <string>
#include "action/action.h"
#include "phevaluator/phevaluator.h"
using Card = phevaluator::Card;

int main() {
  // Action a1 { ActionType::Check };
  // Action a2 { ActionType::Call };
  // std::cout << std::hash<Action>{}(a1) << '\n';
  // std::cout << std::hash<Action>{}(a2) << '\n';
  
  Card c1("Ah");
  std::cout << c1.describeRank() << '\n';
  std::cout << c1.describeSuit() << '\n';
  std::cout << c1.describeCard() << '\n';
}
