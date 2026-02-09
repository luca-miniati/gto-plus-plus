#include <iostream>
#include <string>
#include <unordered_set>
#include <chrono>
#include "tree/tree.h"
#include "action/fixed_abstraction.h"
#include "phevaluator/phevaluator.h"
using Card = phevaluator::Card;

int main() {
  std::vector<Action> actions = {Action(ActionType::Check)};
  std::unique_ptr<ActionAbstraction> abst =
    std::make_unique<FixedAbstraction>(std::move(actions));
  std::vector<Card> flop = {Card("Jh"), Card("9d"), Card("2s")};
  Tree t(6, 3, {100, 100}, std::move(abst), flop);
  std::cout << "building tree..." << std::endl;

  auto start = std::chrono::high_resolution_clock::now();
  t.Build();
  auto end = std::chrono::high_resolution_clock::now();

  std::chrono::duration<double> elapsed = end - start;
  std::cout << "tree built in " << elapsed.count() << " seconds" << std::endl;
  std::cout << "num nodes: " << t.Size() << std::endl;

  auto ik = t.GetKeys();
  std::cout << "num info sets: " << ik.size() << std::endl;

  int F = 0;
  int T = 0;
  int R = 0;
  for (auto [info_set, k] : ik) {
    if (info_set.state.street == Street::Flop && !info_set.state.IsChanceNode())
      ++F;
    else if (info_set.state.street == Street::Turn && !info_set.state.IsChanceNode())
      ++T;
    else if (info_set.state.street == Street::River && !info_set.state.IsChanceNode())
      ++R;
  }
  std::cout << "flop: " << F << std::endl;
  std::cout << "turn: " << T << std::endl;
  std::cout << "river: " << R << std::endl;
}
