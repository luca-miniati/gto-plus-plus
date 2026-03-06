#include <gtest/gtest.h>
#include "action/action.h"
#include "action/fixed_abstraction.h"

namespace {
    const Action CHECK(ActionType::Check);
    const Action CALL(ActionType::Call);
    const Action FOLD(ActionType::Fold);
    const Action B1(ActionType::Bet, 1.0);
    const Action B10(ActionType::Bet, 10.0);
}

TEST(TestAction, TestConstructor) {
  Action a1(ActionType::Bet, 1);
  Action a2(ActionType::Bet, 5);
  Action a3(ActionType::Check);
  Action a4(ActionType::Call);

  ASSERT_TRUE(a1.type == ActionType::Bet);
  ASSERT_TRUE(a2.type == ActionType::Bet);
  ASSERT_TRUE(a3.type == ActionType::Check);
  ASSERT_TRUE(a4.type == ActionType::Call);

  ASSERT_EQ(a1.amount, 1);
  ASSERT_EQ(a2.amount, 5);
  ASSERT_EQ(a3.amount, -1);
  ASSERT_EQ(a4.amount, -1);
}

TEST(TestActionAbstraction, TestFixedAbstraction) {
    auto abst = FixedAbstraction({CHECK, CALL, FOLD, B1});
    ASSERT_EQ(abst.GetActions({}).size(), 4);
}

