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
  // only action is check, bet, call, fold
  std::vector<Action> actions = {
    Action(ActionType::Check),
    Action(ActionType::Bet, 10),
    Action(ActionType::Call),
    Action(ActionType::Fold),
  };

  Cards flop = {Card("Jh"), Card("9h"), Card("2h")};
  auto a_abst = std::make_unique<FixedAbstraction>(std::move(actions));
  auto i_abst_tree = std::make_unique<CanonicalSuitAbstraction>();
  auto i_abst= std::make_unique<CanonicalSuitAbstraction>();

  Tree t(
      /* pot_contributions  =*/ {20, 20},
      /* max_raises         =*/ 1,
      /* starting_stacks    =*/ {80, 80},
      /* action_abst        =*/ std::move(a_abst),
      /* info_set_abst      =*/ std::move(i_abst_tree),
      /* flop               =*/ flop
      );
  std::cout << "building tree..." << std::endl;
  auto start = std::chrono::high_resolution_clock::now();
  t.Build();
  auto end = std::chrono::high_resolution_clock::now();
  auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "elapsed: " << dur << std::endl;
  std::cout << "|t| = " << t.Size() << std::endl;

  // 2 child states: p0 checks or bets the flop
  // ASSERT_EQ(root->GetChildren().size(), 2);
  // NodePtr u = t.GetChild(root, 0);  // take check line

  // 2 child states: p1 checks back or bets
  // ASSERT_EQ(u->GetChildren().size(), 2);
  // u = t.GetChild(u, 1);  // take bet line

  // 2 child states: p0 calls or folds
  // ASSERT_EQ(u->GetChildren().size(), 2);
}
