#include <iostream>
#include "game/game_state.h"
#include "tree/tree.h"
#include "info_set/canonical_suit_abstraction.h"
#include "action/fixed_abstraction.h"

int main() {
  // only action is check
  std::vector<Action> actions = {
      Action(ActionType::Check),
  };

  Cards flop = {Card("Jh"), Card("9h"), Card("2h")};
  GameState state = GameState::InitialState(
      /* pot_contributions  =*/{20, 20},
      /* starting_stacks    =*/{80, 80},
      /* flop               =*/flop);
  auto a_abst = std::make_unique<FixedAbstraction>(std::move(actions));
  auto i_abst_tree = std::make_unique<CanonicalSuitAbstraction>();
  auto i_abst = std::make_unique<CanonicalSuitAbstraction>();

  Tree t(
      /* initial_state      =*/state,
      /* max_raises         =*/1,
      /* action_abst        =*/std::move(a_abst),
      /* info_set_abst      =*/std::move(i_abst_tree));
  t.Build();
  NodeIdx root = t.Root();
}