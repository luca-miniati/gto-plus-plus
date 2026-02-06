#include "gtest/gtest.h"
#include "game/game_state.h"

TEST(TestGameState, TestConstructor) {
  GameState s1 = GameState::InitialState(2 * 3, {100 - 3, 100 - 3});
  GameState s2 = GameState::InitialState(2 * 3, {100 - 3, 100 - 3});

  ASSERT_TRUE(s1 == s2);
  ASSERT_TRUE(std::hash<GameState>{}(s1) == std::hash<GameState>{}(s2));
}
