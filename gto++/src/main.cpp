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

std::ostream& operator<<(std::ostream& os, __uint128_t value) {
  uint64_t high = value >> 64;
  uint64_t low  = static_cast<uint64_t>(value);

  if (high == 0)
    return os << std::hex << low;

  return os << std::hex << high
    << std::setw(16) << std::setfill('0') << low;
}

int main() {
  // std::vector<Card> flop = {Card("Jh"), Card("Td"), Card("2h")};
  // std::vector<Card> hand = {Card("Ah"), Card("Kc")};
  // GameState state = GameState::InitialState(2 * 3, {100, 100}, flop);
  //
  // std::set<int> have;
  // for (auto s1 : SUITS)
  // for (auto s2 : SUITS)
  // for (auto r1 : RANKS)
  // for (auto r2 : RANKS)
  //   have.insert(
  //       abst->GetKey(InfoSet{state, {
  //         Card(std::string(1, r1) + std::string(1, s1)),
  //         Card(std::string(1, r2) + std::string(1, s2))}})
  //       );
  // std::cout << have.size() << '\n';

  std::vector<Action> actions = {Action(ActionType::Check), Action(ActionType::Bet, 2), Action(ActionType::Call)};
  std::unique_ptr<ActionAbstraction> a_abst =
    std::make_unique<FixedAbstraction>(std::move(actions));
  std::unique_ptr<InfoSetAbstraction> i_abst =
    std::make_unique<U128Abstraction>();
  std::vector<Card> flop = {Card("Jh"), Card("9d"), Card("2s")};
  Tree t(6, 3, {100, 100}, std::move(a_abst), std::move(i_abst), flop);
  std::cout << "building tree..." << std::endl;

  auto start = std::chrono::high_resolution_clock::now();
  t.Build();
  auto end = std::chrono::high_resolution_clock::now();

  std::chrono::duration<double> elapsed = end - start;
  std::cout << "tree built in " << elapsed.count() << " seconds" << std::endl;
  std::cout << "num nodes: " << t.Size() << std::endl;

  // auto ik = t.GetKeys();
  // std::cout << "num info sets: " << ik.size() << std::endl;
  //
  // int F = 0;
  // int T = 0;
  // int R = 0;
  // for (auto [info_set, k] : ik) {
  //   if (info_set.state.street == Street::Flop && !info_set.state.IsChanceNode())
  //     ++F;
  //   else if (info_set.state.street == Street::Turn && !info_set.state.IsChanceNode())
  //     ++T;
  //   else if (info_set.state.street == Street::River && !info_set.state.IsChanceNode())
  //     ++R;
  // }
  // std::cout << "flop: " << F << std::endl;
  // std::cout << "turn: " << T << std::endl;
  // std::cout << "river: " << R << std::endl;
}
