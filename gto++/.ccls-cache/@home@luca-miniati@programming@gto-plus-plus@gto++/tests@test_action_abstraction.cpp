#include <gtest/gtest.h>
#include "action.h"
#include "action_abstraction/action_abstraction.h"
#include "action_abstraction/fixed_abstraction.h"
#include "action_abstraction/per_street_abstraction.h"

namespace {
    const Action CHECK(ActionType::Check);
    const Action CALL(ActionType::Call);
    const Action FOLD(ActionType::Fold);
    const Action B1(ActionType::Bet, 1.0);
    const Action B10(ActionType::Bet, 10.0);
}

TEST(TestActionAbstraction, TestFixedAbstraction) {
    auto abst = FixedAbstraction({CHECK, CALL, FOLD, B1});
    ASSERT_EQ(abst.GetActions({}).size(), 4);
}
