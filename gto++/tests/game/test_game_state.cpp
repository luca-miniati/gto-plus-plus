#include "gtest/gtest.h"
#include "game/game_state.h"

TEST(TestGameState, TestConstructor) {
  Deck d1, d2;
  GameState s1 = GameState::InitialState(2 * 3, {100, 100}, d1);
  GameState s2 = GameState::InitialState(2 * 3, {100, 100}, d1);

  ASSERT_TRUE(s1 == s2);
  ASSERT_TRUE(std::hash<GameState>{}(s1) == std::hash<GameState>{}(s2));
}
