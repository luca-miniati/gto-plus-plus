#include <gtest/gtest.h>
#include "action.h"

TEST(TestAction, TestConstructor) {
  Action a1 { ActionType::Bet, 1.0 };
  Action a2 { ActionType::Bet, 5.0 };
  Action a3 { ActionType::Check };
  Action a4 { ActionType::Call };

  ASSERT_TRUE(a1.GetType() == ActionType::Bet);
  ASSERT_TRUE(a2.GetType() == ActionType::Bet);
  ASSERT_TRUE(a3.GetType() == ActionType::Check);
  ASSERT_TRUE(a4.GetType() == ActionType::Call);

  ASSERT_FLOAT_EQ(a1.GetAmount(), 1);
  ASSERT_FLOAT_EQ(a2.GetAmount(), 5);
  ASSERT_FLOAT_EQ(a3.GetAmount(), -1);
  ASSERT_FLOAT_EQ(a4.GetAmount(), -1);
}
