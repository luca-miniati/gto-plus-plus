#include <gtest/gtest.h>
#include "tree/tree.h"
#include "info_set/canonical_suit_abstraction.h"
#include "action/fixed_abstraction.h"
#include "game/game_model.h"
#include "utils/utils.h"

// Helper to find first terminal node in a tree (preorder).
static bool FindFirstTerminal(Tree &t, NodeIdx u, NodeIdx &out) {
  if (t.IsTerminal(u)) {
    out = u;
    return true;
  }
  for (int i = 0; i < t.NumChildren(u); ++i) {
    NodeIdx v = t.Child(u, i);
    if (FindFirstTerminal(t, v, out)) return true;
  }
  return false;
}

TEST(TestTerminalMatrices, ShowdownMatrixReasonableValues) {
  // Simple game: both players always check every street, guaranteeing showdown.
  std::vector<Action> actions = {Action(ActionType::Check)};

  Cards flop = {Card("Jh"), Card("9h"), Card("2h")};
  GameState state = GameState::InitialState(
      std::array<Chips, 2>{20, 20},
      std::array<Chips, 2>{80, 80},
      flop);

  auto action_abst = std::make_unique<FixedAbstraction>(std::move(actions));
  auto info_set_abst = std::make_unique<CanonicalSuitAbstraction>();

  Tree t(state, 1, std::move(action_abst), std::move(info_set_abst));
  t.Build();

  // Find a terminal node and inspect its terminal matrix.
  NodeIdx root = t.Root();
  NodeIdx terminal = root;
  ASSERT_TRUE(FindFirstTerminal(t, root, terminal));

  TerminalIdx tidx = t.TerminalMatrixIndex(terminal);
  ASSERT_NE(tidx, kInvalidTerminal);

  const Eigen::MatrixXd &T = t.TerminalMatrix(tidx);
  ASSERT_GT(T.rows(), 0);
  ASSERT_EQ(T.rows(), T.cols());

  // Entries must be bounded by the pot size.
  double max_abs = 0.0;
  for (int i = 0; i < T.rows(); ++i)
    for (int j = 0; j < T.cols(); ++j)
      max_abs = std::max(max_abs, std::abs(T(i, j)));

  // Pot is at least blinds; use a loose upper bound.
  ASSERT_LE(max_abs, 200.0);
}

