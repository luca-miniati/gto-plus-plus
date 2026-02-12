#include <gtest/gtest.h>
#include "game/game_model.h"
#include "utils/utils.h"

namespace {
    const Action CHECK(ActionType::Check);
    const Action CALL(ActionType::Call);
    const Action FOLD(ActionType::Fold);
}

TEST(TestIsLegal, BetIsLegalWhenNoBetExists) {
  Deck deck;
  std::vector<Card> flop = {Card("Jh"), Card("As"), Card("Kd")};
  GameState state = GameState::InitialState(6, {100, 100}, flop);

  Action bet(ActionType::Bet, 10);

  ASSERT_TRUE(GameModel::IsLegal(state, bet, 5));
}

TEST(TestIsLegal, BetIllegalIfBetAlreadyExists) {
  Deck deck;
  std::vector<Card> flop = {Card("Jh"), Card("As"), Card("Kd")};
  GameState state = GameState::InitialState(6, {100, 100}, flop);
  state.current_bets = {10, 0};
  state.current_player = 1;
  
  Action bet(ActionType::Bet, 20);

  ASSERT_FALSE(GameModel::IsLegal(state, bet, 5));
}

TEST(TestIsLegal, CheckLegalWhenNoBet) {
  Deck deck;
  std::vector<Card> flop = {Card("Jh"), Card("As"), Card("Kd")};
  GameState state = GameState::InitialState(6, {100, 100}, flop);
  
  ASSERT_TRUE(GameModel::IsLegal(state, CHECK, 5));
}

TEST(TestIsLegal, CheckIllegalFacingBet) {
  Deck deck;
  std::vector<Card> flop = {Card("Jh"), Card("As"), Card("Kd")};
  GameState state = GameState::InitialState(6, {100, 100}, flop);
  state.current_bets = {10, 0};
  state.current_stacks = {100 - 3 - 10, 100 - 3};
  state.current_player = 1;
  
  ASSERT_FALSE(GameModel::IsLegal(state, CHECK, 5));
}

TEST(TestIsLegal, CallLegalFacingBet) {
  Deck deck;
  std::vector<Card> flop = {Card("Jh"), Card("As"), Card("Kd")};
  GameState state = GameState::InitialState(6, {100, 100}, flop);
  state.current_bets = {10, 0};
  state.current_stacks = {100 - 3 - 10, 100 - 3};
  state.current_player = 1;
  
  ASSERT_TRUE(GameModel::IsLegal(state, CALL, 5));
}

TEST(TestIsLegal, CallIllegalIfNoBet) {
  Deck deck;
  std::vector<Card> flop = {Card("Jh"), Card("As"), Card("Kd")};
  GameState state = GameState::InitialState(6, {100, 100}, flop);
  
  ASSERT_FALSE(GameModel::IsLegal(state, CALL, 5));
}

TEST(TestIsLegal, RaiseLegalWithProperSize) {
  Deck deck;
  std::vector<Card> flop = {Card("Jh"), Card("As"), Card("Kd")};
  GameState state = GameState::InitialState(6, {100, 100}, flop);
  state.current_bets = {10, 0};
  state.current_stacks = {100 - 3 - 10, 100 - 3};
  state.current_player = 1;
  
  Action raise(ActionType::Raise, 20);

  ASSERT_TRUE(GameModel::IsLegal(state, raise, 5));
}

TEST(TestIsLegal, RaiseIllegalIfTooSmall) {
  Deck deck;
  std::vector<Card> flop = {Card("Jh"), Card("As"), Card("Kd")};
  GameState state = GameState::InitialState(6, {100, 100}, flop);
  state.current_bets = {10, 0};
  state.current_stacks = {100 - 3 - 10, 100 - 3};
  state.current_player = 1;
  
  Action raise(ActionType::Raise, 15);

  ASSERT_FALSE(GameModel::IsLegal(state, raise, 5));
}

TEST(TestIsLegal, RaiseIllegalIfNoBetExists) {
  Deck deck;
  std::vector<Card> flop = {Card("Jh"), Card("As"), Card("Kd")};
  GameState state = GameState::InitialState(6, {100, 100}, flop);
  
  Action raise(ActionType::Raise, 10);

  ASSERT_FALSE(GameModel::IsLegal(state, raise, 5));
}

TEST(TestIsLegal, RaiseLegalIfAllin) {
  Deck deck;
  std::vector<Card> flop = {Card("Jh"), Card("As"), Card("Kd")};
  GameState state = GameState::InitialState(6, {100, 100}, flop);
  state.current_bets = {90, 0};
  state.current_stacks = {100 - 3 - 90, 100 - 3};
  state.current_player = 1;
  
  Action raise(ActionType::Raise, 97);

  ASSERT_TRUE(GameModel::IsLegal(state, raise, 5));
}

TEST(TestStep, BetUpdatesStateCorrectly) {
  Deck deck;
  std::vector<Card> flop = {Card("Jh"), Card("As"), Card("Kd")};
  GameState state = GameState::InitialState(6, {100, 100}, flop);
  Action bet(ActionType::Bet, 10);
  
  GameState next = GameModel::Step(state, bet, 0);
  
  ASSERT_FALSE(next.is_terminal);
  ASSERT_EQ(next.current_player, 1);  // Switches to opponent
  ASSERT_EQ(next.current_raises, 1);
  ASSERT_EQ(next.pot, 6);  // Pot doesn't change until bet is called
  ASSERT_EQ(next.street, Street::Flop);
  ASSERT_EQ(next.current_bets[0], 10);
  ASSERT_EQ(next.current_bets[1], 0);
  ASSERT_EQ(next.current_stacks[0], 100 - 10);  // Starting stack - bet
  ASSERT_EQ(next.current_stacks[1], 100);
  ASSERT_EQ(next.history.size(), 1);
}

TEST(TestStep, BetDecreasesActorStack) {
  Deck deck;
  std::vector<Card> flop = {Card("Jh"), Card("As"), Card("Kd")};
  GameState state = GameState::InitialState(6, {100, 100}, flop);
  Action bet(ActionType::Bet, 25);
  
  GameState next = GameModel::Step(state, bet, 0);
  
  ASSERT_EQ(next.current_stacks[0], 100 - 25);
  ASSERT_EQ(next.current_bets[0], 25);
}

TEST(TestStep, CallOnFlopAdvancesToTurn) {
  Deck deck;
  std::vector<Card> flop = {Card("Jh"), Card("As"), Card("Kd")};
  GameState state = GameState::InitialState(6, {100, 100}, flop);
  state.current_bets = {10, 0};
  state.current_stacks = {100 - 10, 100};
  state.current_player = 1;
  
  GameState next = GameModel::Step(state, CALL, 0);
  
  ASSERT_FALSE(next.is_terminal);
  ASSERT_EQ(next.street, Street::Turn);
  ASSERT_EQ(next.pot, 6 + 2 * 10);  // Original pot + call amount
  ASSERT_EQ(next.current_bets[0], 0);
  ASSERT_EQ(next.current_bets[1], 0);  // Bets reset
  ASSERT_EQ(next.current_stacks[0], 100 - 10);
  ASSERT_EQ(next.current_stacks[1], 100 - 10);
  ASSERT_TRUE(next.IsChanceNode());  // turn needs to be dealt
}

TEST(TestStep, CallOnTurnAdvancesToRiver) {
  Deck deck;
  std::vector<Card> flop = {Card("Jh"), Card("As"), Card("Kd")};
  GameState state = GameState::InitialState(6, {100, 100}, flop);

  state.street = Street::Turn;
  state.current_bets = {15, 0};
  state.current_stacks = {85, 100};
  state.current_player = 1;
  
  GameState next = GameModel::Step(state, CALL, 0);
  
  ASSERT_FALSE(next.is_terminal);
  ASSERT_EQ(next.street, Street::River);
  ASSERT_EQ(next.pot, 6 + 15 * 2);
  ASSERT_EQ(next.current_bets[0], 0);
  ASSERT_EQ(next.current_bets[1], 0);
  ASSERT_TRUE(next.IsChanceNode());  // river needs to be dealt
}

TEST(TestStep, CallOnRiverMakesTerminal) {
  Deck deck;
  std::vector<Card> flop = {Card("Jh"), Card("As"), Card("Kd")};
  GameState state = GameState::InitialState(6, {100, 100}, flop);
  state.street = Street::River;
  state.current_bets = {20, 0};
  state.current_stacks = {80, 100};
  state.current_player = 1;
  
  GameState next = GameModel::Step(state, CALL, 0);
  
  ASSERT_TRUE(next.is_terminal);
  ASSERT_EQ(next.street, Street::River);  // Street doesn't change
  ASSERT_EQ(next.pot, 6 + 20 * 2);
  ASSERT_EQ(next.current_bets[0], 0);
  ASSERT_EQ(next.current_bets[1], 0);
}

TEST(TestStep, CallUpdatesStackCorrectly) {
  Deck deck;
  std::vector<Card> flop = {Card("Jh"), Card("As"), Card("Kd")};
  GameState state = GameState::InitialState(6, {100, 100}, flop);
  state = GameModel::Step(state, Action(ActionType::Bet, 20), 0);
  state = GameModel::Step(state, CALL, 0);
  
  ASSERT_EQ(state.current_stacks[0], 100 - 20);
  ASSERT_EQ(state.current_stacks[1], 100 - 20);
  ASSERT_EQ(state.pot, 2 * (3 + 20));
}

TEST(TestStep, FirstCheckPassesToOpponent) {
  Deck deck;
  std::vector<Card> flop = {Card("Jh"), Card("As"), Card("Kd")};
  GameState state = GameState::InitialState(6, {100, 100}, flop);
  
  GameState next = GameModel::Step(state, CHECK, 0);
  
  ASSERT_FALSE(next.is_terminal);
  ASSERT_EQ(next.current_player, 1);
  ASSERT_EQ(next.street, Street::Flop);  // Street doesn't advance
  ASSERT_EQ(next.pot, 6);
  ASSERT_EQ(next.current_bets[0], 0);
  ASSERT_EQ(next.current_bets[1], 0);
  ASSERT_EQ(next.community_cards.size(), 3);  // No new card
}

TEST(TestStep, SecondCheckOnFlopAdvancesToTurn) {
  Deck deck;
  std::vector<Card> flop = {Card("Jh"), Card("As"), Card("Kd")};
  GameState state = GameState::InitialState(6, {100, 100}, flop);
  state.current_player = 1;
  
  GameState next = GameModel::Step(state, CHECK, 0);
  
  ASSERT_FALSE(next.is_terminal);
  ASSERT_EQ(next.street, Street::Turn);
  ASSERT_EQ(next.pot, 6);
  ASSERT_EQ(next.current_bets[0], 0);
  ASSERT_EQ(next.current_bets[1], 0);
  ASSERT_TRUE(next.IsChanceNode());  // turn needs to be dealt
}

TEST(TestStep, SecondCheckOnTurnAdvancesToRiver) {
  Deck deck;
  std::vector<Card> flop = {Card("Jh"), Card("As"), Card("Kd")};
  GameState state = GameState::InitialState(6, {100, 100}, flop);
  state.street = Street::Turn;
  state.current_player = 1;
  
  GameState next = GameModel::Step(state, CHECK, 0);
  
  ASSERT_FALSE(next.is_terminal);
  ASSERT_EQ(next.street, Street::River);
  ASSERT_TRUE(next.IsChanceNode());  // river needs to be dealt
}

TEST(TestStep, SecondCheckOnRiverMakesTerminal) {
  Deck deck;
  std::vector<Card> flop = {Card("Jh"), Card("As"), Card("Kd")};
  GameState state = GameState::InitialState(6, {100, 100}, flop);
  state.street = Street::River;
  state.current_player = 1;
  
  GameState next = GameModel::Step(state, CHECK, 0);
  
  ASSERT_TRUE(next.is_terminal);
  ASSERT_EQ(next.street, Street::River);
}

TEST(TestStep, FoldMakesGameTerminal) {
  Deck deck;
  std::vector<Card> flop = {Card("Jh"), Card("As"), Card("Kd")};
  GameState state = GameState::InitialState(6, {100, 100}, flop);
  state.current_bets = {20, 0};
  state.pot += 20;
  state.current_stacks = {77, 97};
  state.current_player = 1;
  
  GameState next = GameModel::Step(state, FOLD, 0);
  
  ASSERT_TRUE(next.is_terminal);
  ASSERT_EQ(next.pot, 6 + 20);  // Pot doesn't change on fold
  ASSERT_EQ(next.street, state.street);
  ASSERT_EQ(next.current_stacks[0], 77);
  ASSERT_EQ(next.current_stacks[1], 97);  // Stacks don't change
}

TEST(TestStep, RaiseUpdatesStateCorrectly) {
  // testcase 1
  Deck deck;
  std::vector<Card> flop = {Card("Jh"), Card("As"), Card("Kd")};
  GameState state = GameState::InitialState(6, {100, 100}, flop);

  state = GameModel::Step(state, Action(ActionType::Bet, 10), 0);
  state = GameModel::Step(state, Action(ActionType::Raise, 20), 0);
  
  ASSERT_FALSE(state.is_terminal);
  ASSERT_EQ(state.current_player, 0);
  ASSERT_EQ(state.current_raises, 2);
  ASSERT_EQ(state.pot, 6);
  ASSERT_EQ(state.current_bets[0], 10);
  ASSERT_EQ(state.current_bets[1], 20);
  ASSERT_EQ(state.current_stacks[0], 100 - 10);
  ASSERT_EQ(state.current_stacks[1], 100 - 20);

  // testcase 2
  deck = Deck();
  flop = {Card("Jh"), Card("As"), Card("Kd")};
  state = GameState::InitialState(6, {100, 100}, flop);

  state = GameModel::Step(state, Action(ActionType::Bet, 10), 0);
  state = GameModel::Step(state, Action(ActionType::Raise, 20), 0);
  state = GameModel::Step(state, Action(ActionType::Raise, 40), 0);
  
  ASSERT_FALSE(state.is_terminal);
  ASSERT_EQ(state.current_player, 1);
  ASSERT_EQ(state.current_raises, 3);
  ASSERT_EQ(state.pot, 6);
  ASSERT_EQ(state.current_bets[0], 40);
  ASSERT_EQ(state.current_bets[1], 20);
  ASSERT_EQ(state.current_stacks[0], 100 - 40);
  ASSERT_EQ(state.current_stacks[1], 100 - 20);
}

TEST(TestStep, RaiseAfterRaiseIncrementsRaiseCount) {
  Deck deck;
  std::vector<Card> flop = {Card("Jh"), Card("As"), Card("Kd")};
  GameState state = GameState::InitialState(6, {100, 100}, flop);

  Action bet1(ActionType::Bet, 3);
  state = GameModel::Step(state, bet1, 0);
  
  Action raise(ActionType::Raise, 12);  // raise to 12
  state = GameModel::Step(state, raise, 0);
  
  ASSERT_EQ(state.current_raises, 2);
  ASSERT_EQ(state.pot, 6);
  ASSERT_EQ(state.current_bets[0], 3);
  ASSERT_EQ(state.current_bets[1], 12);
  ASSERT_EQ(state.current_stacks[0], 100 - 3);
  ASSERT_EQ(state.current_stacks[1], 100 - 12);
}

TEST(TestStep, RaiseAllInScenario) {
  Deck deck;
  std::vector<Card> flop = {Card("Jh"), Card("As"), Card("Kd")};
  GameState state = GameState::InitialState(6, {100, 100}, flop);

  Action bet(ActionType::Bet, 20);
  state = GameModel::Step(state, bet, 0);
  
  Action raise(ActionType::Raise, 100);
  state = GameModel::Step(state, raise, 0);
  
  ASSERT_EQ(state.current_stacks[1], 0);
  ASSERT_EQ(state.current_bets[1], 100);
}

TEST(TestStep, HistoryIsUpdatedWithAction) {
  Deck deck;
  std::vector<Card> flop = {Card("Jh"), Card("As"), Card("Kd")};
  GameState state = GameState::InitialState(6, {100, 100}, flop);
  Action bet(ActionType::Bet, 15);
  
  GameState next = GameModel::Step(state, bet, 17);
  
  ASSERT_EQ(next.history.size(), 1);
  ASSERT_EQ(next.history[0], 17);
}

TEST(TestStep, HistoryAccumulatesActions) {
  Deck deck;
  std::vector<Card> flop = {Card("Jh"), Card("As"), Card("Kd")};
  GameState state = GameState::InitialState(6, {100, 100}, flop);
  state = GameModel::Step(state, Action(ActionType::Bet, 10), 2);
  state = GameModel::Step(state, CALL, 3);
  
  ASSERT_EQ(state.history.size(), 2);
  ASSERT_EQ(state.history[0], 2);
  ASSERT_EQ(state.history[1], 3);
}

TEST(TestStep, ThrowsOnTerminalState) {
  Deck deck;
  std::vector<Card> flop = {Card("Jh"), Card("As"), Card("Kd")};
  GameState state = GameState::InitialState(6, {100, 100}, flop);
  state.is_terminal = true;
  
  ASSERT_THROW(GameModel::Step(state, CHECK, 0), std::runtime_error);
}
