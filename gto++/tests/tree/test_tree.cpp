#include <gtest/gtest.h>
#include <unordered_set>
#include "action/fixed_abstraction.h"
#include "info_set/canonical_suit_abstraction.h"
#include "tree/tree.h"
#include "utils/utils.h"

int NumLeaves(Tree &t, NodePtr u) {
  if (u->IsTerminal())
    return 1;
  int ans = 0;
  for (NodeIdx i : u->GetChildren())
    ans += NumLeaves(t, t.GetNode(i));
  return ans;
}

TEST(TestTree, TestBuildTrivialTree) {
  // only action is check
  std::vector<Action> actions = {
    Action(ActionType::Check),
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
  t.Build();
  NodePtr root = t.GetRoot();

  // one child state: p0 checks the flop
  ASSERT_EQ(root->GetChildren().size(), 1);
  NodePtr u = t.GetChild(root, 0);

  // one child state: p1 checks back
  ASSERT_EQ(u->GetChildren().size(), 1);
  u = t.GetChild(u, 0);

  // child states:
  // deal heart: {2h, 3h, ..., Ah} - {2h, 9h, Jh}: 10 states
  // deal non-heart: {2, 3, ..., A}: 13 states
  ASSERT_EQ(u->GetChildren().size(), 10 + 13);

  // count up terminal states
  int actual = NumLeaves(t, root);

  // should be exactly the # of runouts under suit isomorphism
  std::unordered_set<PublicInfoKey> canonical_rivers;
  for (Card c1 : CARDS) {
    for (Card c2 : CARDS) {
      if (c1 != c2 &&
          !std::ranges::contains(flop, c1) &&
          !std::ranges::contains(flop, c2)) {
        Cards cards = {flop[0], flop[1], flop[2], c1, c2};
        PublicInfoKey key = i_abst->GetPublicInfoKey(
            cards, 
            {0, 0, 0, 0, 0, 0}
            );
        canonical_rivers.insert(key);
      }
    }
  }
  int expected = canonical_rivers.size();

  ASSERT_EQ(expected, actual);
}
