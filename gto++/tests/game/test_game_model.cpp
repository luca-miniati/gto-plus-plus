#include <gtest/gtest.h>
#include "game/game_model.h"

namespace {
    const Action CHECK(ActionType::Check);
    const Action CALL(ActionType::Call);
    const Action FOLD(ActionType::Fold);
}

TEST(TestIsLegal, BetIsLegalWhenNoBetExists) {
  GameState state = GameState::InitialState(6, {100, 100});

  Action bet(ActionType::Bet, 10);

  ASSERT_TRUE(GameModel::IsLegal(state, bet));
}

TEST(TestIsLegal, BetIllegalIfBetAlreadyExists) {
  GameState state = GameState::InitialState(6, {100, 100});
  state.current_bets = {10, 0};
  state.current_player = 1;
  
  Action bet(ActionType::Bet, 20);

  ASSERT_FALSE(GameModel::IsLegal(state, bet));
}

TEST(TestIsLegal, CheckLegalWhenNoBet) {
  GameState state = GameState::InitialState(6, {100, 100});
  
  ASSERT_TRUE(GameModel::IsLegal(state, CHECK));
}

TEST(TestIsLegal, CheckIllegalFacingBet) {
  GameState state = GameState::InitialState(6, {100, 100});
  state.current_bets = {10, 0};
  state.current_stacks = {100 - 3 - 10, 100 - 3};
  state.current_player = 1;
  
  ASSERT_FALSE(GameModel::IsLegal(state, CHECK));
}

TEST(TestIsLegal, CallLegalFacingBet) {
  GameState state = GameState::InitialState(6, {100, 100});
  state.current_bets = {10, 0};
  state.current_stacks = {100 - 3 - 10, 100 - 3};
  state.current_player = 1;
  
  ASSERT_TRUE(GameModel::IsLegal(state, CALL));
}

TEST(TestIsLegal, CallIllegalIfNoBet) {
  GameState state = GameState::InitialState(6, {100, 100});
  
  ASSERT_FALSE(GameModel::IsLegal(state, CALL));
}

TEST(TestIsLegal, RaiseLegalWithProperSize) {
  GameState state = GameState::InitialState(6, {100, 100});
  state.current_bets = {10, 0};
  state.current_stacks = {100 - 3 - 10, 100 - 3};
  state.current_player = 1;
  
  Action raise(ActionType::Raise, 20);

  ASSERT_TRUE(GameModel::IsLegal(state, raise));
}

TEST(TestIsLegal, RaiseIllegalIfTooSmall) {
  GameState state = GameState::InitialState(6, {100, 100});
  state.current_bets = {10, 0};
  state.current_stacks = {100 - 3 - 10, 100 - 3};
  state.current_player = 1;
  
  Action raise(ActionType::Raise, 15);

  ASSERT_FALSE(GameModel::IsLegal(state, raise));
}

TEST(TestIsLegal, RaiseIllegalIfNoBetExists) {
  GameState state = GameState::InitialState(6, {100, 100});
  
  Action raise(ActionType::Raise, 10);

  ASSERT_FALSE(GameModel::IsLegal(state, raise));
}

TEST(TestIsLegal, RaiseLegalIfAllin) {
  GameState state = GameState::InitialState(6, {100, 100});
  state.current_bets = {90, 0};
  state.current_stacks = {100 - 3 - 90, 100 - 3};
  state.current_player = 1;
  
  Action raise(ActionType::Raise, 97);

  ASSERT_TRUE(GameModel::IsLegal(state, raise));
}
