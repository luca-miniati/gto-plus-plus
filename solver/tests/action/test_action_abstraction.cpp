#include <gtest/gtest.h>
#include "action/action.h"
#include "action/fixed_abstraction.h"
#include "action/check_fold_abstraction.h"
#include "game/game_model.h"

namespace {
    const Action CHECK(ActionType::Check);
    const Action CALL(ActionType::Call);
    const Action FOLD(ActionType::Fold);
    const Action B1(ActionType::Bet, 1.0);
    const Action B10(ActionType::Bet, 10.0);
}

TEST(TestActionAbstraction, TestCheckFoldAbstraction) {
    auto abst = CheckFoldAbstraction();
    auto state = GameState::InitialState(
        /* pot_contributions  =*/ {3, 3},
        /* starting_stacks    =*/ {200, 200},
        /* flop               =*/ {Card("Ac"), Card("Ad"), Card("Ah")}
        );
    ASSERT_EQ(abst.GetActions(state).size(), 1);
    ASSERT_EQ(abst.GetActions(state)[0], CHECK);

    state = GameModel::Step(state, B1, 0);

    ASSERT_EQ(abst.GetActions(state).size(), 1);
    ASSERT_EQ(abst.GetActions(state)[0], FOLD);

    state = GameModel::Step(state, CALL, 0);

    ASSERT_EQ(abst.GetActions(state).size(), 1);
    ASSERT_EQ(abst.GetActions(state)[0], CHECK);
}

