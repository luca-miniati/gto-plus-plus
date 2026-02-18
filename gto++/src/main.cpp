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
  // only action is check
  std::vector<Action> actions = {
    Action(ActionType::Check),
  };

  Cards flop = {Card("Jh"), Card("9h"), Card("2h")};
  auto a_abst = std::make_unique<FixedAbstraction>(std::move(actions));
  auto i_abst = std::make_unique<CanonicalSuitAbstraction>();

  Tree t(
      /* pot_contributions  =*/ {20, 20},
      /* max_raises         =*/ 1,
      /* starting_stacks    =*/ {80, 80},
      /* action_abst        =*/ std::move(a_abst),
      /* info_set_abst      =*/ std::move(i_abst),
      /* flop               =*/ flop
      );
  t.Build();
  NodePtr root = t.GetRoot();

  std::cout << "|t| = " << t.Size() << '\n';
}
