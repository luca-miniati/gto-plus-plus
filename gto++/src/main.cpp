#include <iostream>
#include <string>
#include <unordered_set>
#include <chrono>
#include <set>
#include "tree/tree.h"
#include "game/game_model.h"
#include "solver/solver.h"
#include "action/fixed_abstraction.h"
#include "info_set/canonical_suit_abstraction.h"
#include "phevaluator/phevaluator.h"
using Card = phevaluator::Card;

int main() {
  std::vector<Action> actions = {
    Action(ActionType::Check),
    Action(ActionType::Call),
    Action(ActionType::Fold),
    Action(ActionType::Bet, 80)
  };

  // std::vector<Card> flop = {Card("Jh"), Card("9d"), Card("2s")};
  std::vector<Card> flop = {Card("Jh"), Card("9h"), Card("2h")};
  std::unique_ptr<ActionAbstraction> a_abst =
    std::make_unique<FixedAbstraction>(std::move(actions));

  std::unique_ptr<InfoSetAbstraction> i_abst =
    std::make_unique<CanonicalSuitAbstraction>();


  std::cout << "building tree:" << std::endl;

  auto start = std::chrono::high_resolution_clock::now();

  Solver s({20, 20}, 1, {80, 80}, std::move(a_abst), std::move(i_abst), flop);
  s.Solve();
  // Tree t({20, 20}, 1, {80, 80}, std::move(a_abst), std::move(i_abst), flop);
  // t.Build();

  auto end = std::chrono::high_resolution_clock::now();

  std::chrono::duration<double> elapsed = end - start;
  std::cout << "elapsed: " << elapsed.count() << " seconds" << std::endl;
}
