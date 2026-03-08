#include "gtest/gtest.h"
#include "game/game_state.h"

TEST(TestGameState, TestConstructor) {
  GameState s1 = GameState::InitialState(
      {3, 3},
      {100, 100},
      {Card("Jh"), Card("9d"), Card("2s")});
  GameState s2 = GameState::InitialState(
      {3, 3},
      {100, 100},
      {Card("Jh"), Card("9d"), Card("2s")});

  ASSERT_TRUE(s1 == s2);
  ASSERT_TRUE(std::hash<GameState>{}(s1) == std::hash<GameState>{}(s2));
}
