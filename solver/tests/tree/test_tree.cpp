#include <gtest/gtest.h>
#include <unordered_set>
#include <vector>
#include "action/fixed_abstraction.h"
#include "info_set/canonical_suit_abstraction.h"
#include "tree/tree.h"
#include "utils/utils.h"

// Count unique terminal nodes reachable from u. Uses memoization because the
// tree is a DAG (shared nodes from canonical suit abstraction); without it,
// traversal would re-visit shared subtrees exponentially and hang.
static int NumLeaves(Tree& t, NodeIdx u, std::vector<int>& cache) {
  if (cache[u] >= 0)
    return cache[u];
  if (t.IsTerminal(u)) {
    cache[u] = 1;
    return 1;
  }
  int ans = 0;
  for (int i = 0; i < t.NumChildren(u); ++i)
    ans += NumLeaves(t, t.Child(u, i), cache);
  cache[u] = ans;
  return ans;
}

TEST(TestTree, TestTrivialTree) {
  // only action is check
  std::vector<Action> actions = {
      Action(ActionType::Check),
  };

  Cards flop = {Card("Jh"), Card("9h"), Card("2h")};
  GameState state = GameState::InitialState(
      /* pot_contributions  =*/std::array<Chips, 2>{20, 20},
      /* starting_stacks    =*/std::array<Chips, 2>{80, 80},
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

  // one child state: p0 checks the flop
  ASSERT_EQ(t.NumChildren(root), 1);
  NodeIdx u = t.Child(root, 0);

  // one child state: p1 checks back
  ASSERT_EQ(t.NumChildren(u), 1);
  NodeIdx v = t.Child(u, 0);

  // child states:
  // deal heart: {2h, 3h, ..., Ah} - {2h, 9h, Jh}: 10 states
  // deal non-heart: {2, 3, ..., A}: 13 states
  ASSERT_EQ(t.NumChildren(v), 10 + 13);

  // count up terminal states
  std::vector<int> cache(t.Size(), -1);
  int actual = NumLeaves(t, t.Root(), cache);

  // should be exactly the # of runouts under suit isomorphism
  std::unordered_set<PublicInfoKey> canonical_rivers;
  for (Card c1 : CARDS) {
    for (Card c2 : CARDS) {
      if (c1 != c2 &&
          !Contains(flop, c1) &&
          !Contains(flop, c2)) {
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

TEST(TestTree, TestSmallTree) {
  // only action is check, bet, call, fold
  std::vector<Action> actions = {
    Action(ActionType::Check),
    Action(ActionType::Bet, 10),
    Action(ActionType::Call),
    Action(ActionType::Fold),
  };

  Cards flop = {Card("Jh"), Card("9h"), Card("2h")};
  GameState state = GameState::InitialState(
      /* pot_contributions  =*/std::array<Chips, 2>{20, 20},
      /* starting_stacks    =*/std::array<Chips, 2>{80, 80},
      /* flop               =*/flop);
  auto action_abst = std::make_unique<FixedAbstraction>(std::move(actions));
  auto info_set_abst_tree = std::make_unique<CanonicalSuitAbstraction>();
  auto info_set_abst= std::make_unique<CanonicalSuitAbstraction>();

  Tree t(
      /* pot_contributions  =*/state,
      /* max_raises         =*/1,
      /* action_abst        =*/std::move(action_abst),
      /* info_set_abst      =*/std::move(info_set_abst_tree));
  t.Build();
  NodeIdx root = t.Root();

  // 2 child states: p0 checks or bets the flop
  ASSERT_EQ(t.NumChildren(root), 2);
  NodeIdx u = t.Child(root, 0);  // take check line

  // 2 child states: p1 checks back or bets
  ASSERT_EQ(t.NumChildren(u), 2);
  NodeIdx v0 = t.Child(u, 0);  // take check line
  NodeIdx v1 = t.Child(u, 1);  // take bet line

  // v0 is a chance node, 23 different turns can be dealt
  ASSERT_EQ(t.NumChildren(v0), 23);
  // 2 child states: p0 calls or folds
  ASSERT_EQ(t.NumChildren(v1), 2);
}

TEST(TestTree, DeterministicBuild) {
  std::vector<Action> actions = {
      Action(ActionType::Check),
      Action(ActionType::Bet, 10),
      Action(ActionType::Call),
      Action(ActionType::Fold),
  };

  Cards flop = {Card("Jh"), Card("9h"), Card("2h")};
  GameState state = GameState::InitialState(
      std::array<Chips, 2>{20, 20},
      std::array<Chips, 2>{80, 80},
      flop);
  auto action_abst1 = std::make_unique<FixedAbstraction>(actions);
  auto action_abst2 = std::make_unique<FixedAbstraction>(std::move(actions));
  auto info_set_abst1 = std::make_unique<CanonicalSuitAbstraction>();
  auto info_set_abst2 = std::make_unique<CanonicalSuitAbstraction>();

  Tree t1(state, 1, std::move(action_abst1), std::move(info_set_abst1));
  Tree t2(state, 1, std::move(action_abst2), std::move(info_set_abst2));
  t1.Build();
  t2.Build();

  ASSERT_EQ(t1.Size(), t2.Size());

  // Simple structural check: for the first few nodes, child counts agree.
  for (NodeIdx i = 0; i < std::min<std::size_t>(t1.Size(), 32); ++i) {
    ASSERT_EQ(t1.NumChildren(i), t2.NumChildren(i));
  }
}
