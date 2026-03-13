#include "gtest/gtest.h"
#include "game/game_state.h"
#include "utils/utils.h"

TEST(TestGameState, TestConstructor) {
  GameState s1 = GameState::InitialState(
      std::array<Chips, 2>{3, 3},
      std::array<Chips, 2>{100, 100},
      {Card("Jh"), Card("9d"), Card("2s")});
  GameState s2 = GameState::InitialState(
      std::array<Chips, 2>{3, 3},
      std::array<Chips, 2>{100, 100},
      {Card("Jh"), Card("9d"), Card("2s")});

  ASSERT_TRUE(s1 == s2);
  ASSERT_TRUE(std::hash<GameState>{}(s1) == std::hash<GameState>{}(s2));
}
