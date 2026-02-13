#include <iostream>
#include <string>
#include <unordered_set>
#include <chrono>
#include <set>
#include "tree/tree.h"
#include "action/fixed_abstraction.h"
#include "info_set/info_set_abstraction.h"
#include "phevaluator/phevaluator.h"
using Card = phevaluator::Card;

int main() {
  std::vector<Action> actions = {
    Action(ActionType::Check),
    Action(ActionType::Bet, 2),
    Action(ActionType::Raise, 10),
    Action(ActionType::Call)
  };
  std::unique_ptr<ActionAbstraction> a_abst =
    std::make_unique<FixedAbstraction>(std::move(actions));
  std::unique_ptr<InfoSetAbstraction> i_abst =
    std::make_unique<U128Abstraction>();

  std::vector<Card> flop = {Card("Jh"), Card("9d"), Card("2s")};
  Tree t(6, 3, {100, 100}, std::move(a_abst), std::move(i_abst), flop);

  GameState state = GameState::InitialState(2 * 3, {100, 100}, flop);
  std::vector<Card> hand = {Card("Ah"), Card("Kc")};

  auto start = std::chrono::high_resolution_clock::now();
  // NodeIdx i = t.GetOrCreateNodeIdx(state);
  t.Build();
  auto end = std::chrono::high_resolution_clock::now();

  std::chrono::duration<double> elapsed = end - start;
  std::cout << "elapsed: " << elapsed.count() << " seconds" << std::endl;
  std::cout << "num nodes: " << t.Size() << "\n";
}
