#include "action/action.h"
#include "tree/tree.h"
#include "info_set/canonical_suit_abstraction.h"
#include "action/fixed_abstraction.h"

int main() {
  // only action is check, bet, call, fold
  std::vector<Action> actions = {
    Action(ActionType::Check),
    Action(ActionType::Bet, 10),
    Action(ActionType::Call),
    Action(ActionType::Fold),
  };

  Cards flop = {Card("Jh"), Card("9h"), Card("2h")};
  GameState state = GameState::InitialState(
      /* pot_contributions  =*/{20, 20},
      /* starting_stacks    =*/{80, 80},
      /* flop               =*/flop);
  auto a_abst = std::make_unique<FixedAbstraction>(std::move(actions));
  auto i_abst_tree = std::make_unique<CanonicalSuitAbstraction>();
  auto i_abst= std::make_unique<CanonicalSuitAbstraction>();

  Tree t(
      /* pot_contributions  =*/state,
      /* max_raises         =*/1,
      /* action_abst        =*/std::move(a_abst),
      /* info_set_abst      =*/std::move(i_abst_tree));
  t.Build();
  NodeIdx root = t.Root();

  // 2 child states: p0 checks or bets the flop
  std::cout << t.NumChildren(root) << std::endl;  // 2
  NodeIdx u = t.Child(root, 0);  // take check line

  // 2 child states: p1 checks back or bets
  std::cout << t.NumChildren(u) << std::endl;  // 2
  NodeIdx v0 = t.Child(u, 0);  // take check line
  NodeIdx v1 = t.Child(u, 1);  // take bet line

  // v0 is a chance node, 23 different turns can be dealt
  std::cout << t.NumChildren(v0) << std::endl;  // 23
  // 2 child states: p0 calls or folds
  std::cout << t.NumChildren(v1) << std::endl; // 2
}