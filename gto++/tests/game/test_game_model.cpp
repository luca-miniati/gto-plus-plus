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
  GameState state = GameState::InitialState(6, {100, 100}, deck);

  Action bet(ActionType::Bet, 10);

  ASSERT_TRUE(GameModel::IsLegal(state, bet));
}

TEST(TestIsLegal, BetIllegalIfBetAlreadyExists) {
  Deck deck;
  GameState state = GameState::InitialState(6, {100, 100}, deck);
  state.current_bets = {10, 0};
  state.current_player = 1;
  
  Action bet(ActionType::Bet, 20);

  ASSERT_FALSE(GameModel::IsLegal(state, bet));
}

TEST(TestIsLegal, CheckLegalWhenNoBet) {
  Deck deck;
  GameState state = GameState::InitialState(6, {100, 100}, deck);
  
  ASSERT_TRUE(GameModel::IsLegal(state, CHECK));
}

TEST(TestIsLegal, CheckIllegalFacingBet) {
  Deck deck;
  GameState state = GameState::InitialState(6, {100, 100}, deck);
  state.current_bets = {10, 0};
  state.current_stacks = {100 - 3 - 10, 100 - 3};
  state.current_player = 1;
  
  ASSERT_FALSE(GameModel::IsLegal(state, CHECK));
}

TEST(TestIsLegal, CallLegalFacingBet) {
  Deck deck;
  GameState state = GameState::InitialState(6, {100, 100}, deck);
  state.current_bets = {10, 0};
  state.current_stacks = {100 - 3 - 10, 100 - 3};
  state.current_player = 1;
  
  ASSERT_TRUE(GameModel::IsLegal(state, CALL));
}

TEST(TestIsLegal, CallIllegalIfNoBet) {
  Deck deck;
  GameState state = GameState::InitialState(6, {100, 100}, deck);
  
  ASSERT_FALSE(GameModel::IsLegal(state, CALL));
}

TEST(TestIsLegal, RaiseLegalWithProperSize) {
  Deck deck;
  GameState state = GameState::InitialState(6, {100, 100}, deck);
  state.current_bets = {10, 0};
  state.current_stacks = {100 - 3 - 10, 100 - 3};
  state.current_player = 1;
  
  Action raise(ActionType::Raise, 20);

  ASSERT_TRUE(GameModel::IsLegal(state, raise));
}

TEST(TestIsLegal, RaiseIllegalIfTooSmall) {
  Deck deck;
  GameState state = GameState::InitialState(6, {100, 100}, deck);
  state.current_bets = {10, 0};
  state.current_stacks = {100 - 3 - 10, 100 - 3};
  state.current_player = 1;
  
  Action raise(ActionType::Raise, 15);

  ASSERT_FALSE(GameModel::IsLegal(state, raise));
}

TEST(TestIsLegal, RaiseIllegalIfNoBetExists) {
  Deck deck;
  GameState state = GameState::InitialState(6, {100, 100}, deck);
  
  Action raise(ActionType::Raise, 10);

  ASSERT_FALSE(GameModel::IsLegal(state, raise));
}

TEST(TestIsLegal, RaiseLegalIfAllin) {
  Deck deck;
  GameState state = GameState::InitialState(6, {100, 100}, deck);
  state.current_bets = {90, 0};
  state.current_stacks = {100 - 3 - 90, 100 - 3};
  state.current_player = 1;
  
  Action raise(ActionType::Raise, 97);

  ASSERT_TRUE(GameModel::IsLegal(state, raise));
}

TEST(TestStep, BetUpdatesStateCorrectly) {
  Deck deck;
  GameState state = GameState::InitialState(6, {100, 100}, deck);
  Action bet(ActionType::Bet, 10);
  
  GameState next = GameModel::Step(state, bet);
  
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
  ASSERT_EQ(next.history[0].type, ActionType::Bet);
}

TEST(TestStep, BetDecreasesActorStack) {
  Deck deck;
  GameState state = GameState::InitialState(6, {100, 100}, deck);
  Action bet(ActionType::Bet, 25);
  
  GameState next = GameModel::Step(state, bet);
  
  ASSERT_EQ(next.current_stacks[0], 100 - 25);
  ASSERT_EQ(next.current_bets[0], 25);
}

TEST(TestStep, CallOnFlopAdvancesToTurn) {
  Deck deck;
  GameState state = GameState::InitialState(6, {100, 100}, deck);
  state.current_bets = {10, 0};
  state.current_stacks = {100 - 10, 100};
  state.current_player = 1;
  
  GameState next = GameModel::Step(state, CALL);
  
  ASSERT_FALSE(next.is_terminal);
  ASSERT_EQ(next.street, Street::Turn);
  ASSERT_EQ(next.pot, 6 + 2 * 10);  // Original pot + call amount
  ASSERT_EQ(next.current_bets[0], 0);
  ASSERT_EQ(next.current_bets[1], 0);  // Bets reset
  ASSERT_EQ(next.current_stacks[0], 100 - 10);
  ASSERT_EQ(next.current_stacks[1], 100 - 10);
  ASSERT_EQ(next.community_cards.size(), 4);  // Flop (3) + Turn (1)
}

TEST(TestStep, CallOnTurnAdvancesToRiver) {
  Deck deck;
  GameState state = GameState::InitialState(6, {100, 100}, deck);
  state.street = Street::Turn;
  state.current_bets = {15, 0};
  state.current_stacks = {85, 100};
  state.current_player = 1;
  state.community_cards = {deck.Pop(), deck.Pop(), deck.Pop(), deck.Pop()};
  
  GameState next = GameModel::Step(state, CALL);
  
  ASSERT_FALSE(next.is_terminal);
  ASSERT_EQ(next.street, Street::River);
  ASSERT_EQ(next.pot, 6 + 15 * 2);
  ASSERT_EQ(next.current_bets[0], 0);
  ASSERT_EQ(next.current_bets[1], 0);
  ASSERT_EQ(next.community_cards.size(), 5);  // Turn (4) + River (1)
}

TEST(TestStep, CallOnRiverMakesTerminal) {
  Deck deck;
  GameState state = GameState::InitialState(6, {100, 100}, deck);
  state.street = Street::River;
  state.current_bets = {20, 0};
  state.current_stacks = {80, 100};
  state.current_player = 1;
  state.community_cards = {deck.Pop(), deck.Pop(), deck.Pop(), 
                          deck.Pop(), deck.Pop()};
  
  GameState next = GameModel::Step(state, CALL);
  
  ASSERT_TRUE(next.is_terminal);
  ASSERT_EQ(next.street, Street::River);  // Street doesn't change
  ASSERT_EQ(next.pot, 6 + 20 * 2);
  ASSERT_EQ(next.current_bets[0], 0);
  ASSERT_EQ(next.current_bets[1], 0);
  ASSERT_EQ(next.community_cards.size(), 5);  // No new card dealt
}

TEST(TestStep, CallUpdatesStackCorrectly) {
  Deck deck;
  GameState state = GameState::InitialState(6, {100, 100}, deck);
  state = GameModel::Step(state, Action(ActionType::Bet, 20));
  state = GameModel::Step(state, CALL);
  
  ASSERT_EQ(state.current_stacks[0], 100 - 20);
  ASSERT_EQ(state.current_stacks[1], 100 - 20);
  ASSERT_EQ(state.pot, 2 * (3 + 20));
}

TEST(TestStep, FirstCheckPassesToOpponent) {
  Deck deck;
  GameState state = GameState::InitialState(6, {100, 100}, deck);
  
  GameState next = GameModel::Step(state, CHECK);
  
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
  GameState state = GameState::InitialState(6, {100, 100}, deck);
  state.current_player = 1;
  state.history = {CHECK};
  
  GameState next = GameModel::Step(state, CHECK);
  
  ASSERT_FALSE(next.is_terminal);
  ASSERT_EQ(next.street, Street::Turn);
  ASSERT_EQ(next.pot, 6);
  ASSERT_EQ(next.current_bets[0], 0);
  ASSERT_EQ(next.current_bets[1], 0);
  ASSERT_EQ(next.community_cards.size(), 4);
}

TEST(TestStep, SecondCheckOnTurnAdvancesToRiver) {
  Deck deck;
  GameState state = GameState::InitialState(6, {100, 100}, deck);
  state.street = Street::Turn;
  state.current_player = 1;
  state.history = {CHECK};
  state.community_cards = {deck.Pop(), deck.Pop(), deck.Pop(), deck.Pop()};
  
  GameState next = GameModel::Step(state, CHECK);
  
  ASSERT_FALSE(next.is_terminal);
  ASSERT_EQ(next.street, Street::River);
  ASSERT_EQ(next.community_cards.size(), 5);
}

TEST(TestStep, SecondCheckOnRiverMakesTerminal) {
  Deck deck;
  GameState state = GameState::InitialState(6, {100, 100}, deck);
  state.street = Street::River;
  state.current_player = 1;
  state.history = {CHECK};
  state.community_cards = {deck.Pop(), deck.Pop(), deck.Pop(), 
                          deck.Pop(), deck.Pop()};
  
  GameState next = GameModel::Step(state, CHECK);
  
  ASSERT_TRUE(next.is_terminal);
  ASSERT_EQ(next.street, Street::River);
  ASSERT_EQ(next.community_cards.size(), 5);  // No new card
}

TEST(TestStep, FoldMakesGameTerminal) {
  Deck deck;
  GameState state = GameState::InitialState(6, {100, 100}, deck);
  state.current_bets = {20, 0};
  state.pot += 20;
  state.current_stacks = {77, 97};
  state.current_player = 1;
  
  GameState next = GameModel::Step(state, FOLD);
  
  ASSERT_TRUE(next.is_terminal);
  ASSERT_EQ(next.pot, 6 + 20);  // Pot doesn't change on fold
  ASSERT_EQ(next.street, state.street);
  ASSERT_EQ(next.current_stacks[0], 77);
  ASSERT_EQ(next.current_stacks[1], 97);  // Stacks don't change
}

TEST(TestStep, RaiseUpdatesStateCorrectly) {
  // testcase 1
  Deck deck;
  GameState state = GameState::InitialState(6, {100, 100}, deck);

  state = GameModel::Step(state, Action(ActionType::Bet, 10));
  state = GameModel::Step(state, Action(ActionType::Raise, 20));
  
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
  state = GameState::InitialState(6, {100, 100}, deck);

  state = GameModel::Step(state, Action(ActionType::Bet, 10));
  state = GameModel::Step(state, Action(ActionType::Raise, 20));
  state = GameModel::Step(state, Action(ActionType::Raise, 40));
  
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
  GameState state = GameState::InitialState(6, {100, 100}, deck);

  Action bet1(ActionType::Bet, 3);
  state = GameModel::Step(state, bet1);
  
  Action raise(ActionType::Raise, 12);  // raise to 12
  state = GameModel::Step(state, raise);
  
  ASSERT_EQ(state.current_raises, 2);
  ASSERT_EQ(state.pot, 6);
  ASSERT_EQ(state.current_bets[0], 3);
  ASSERT_EQ(state.current_bets[1], 12);
  ASSERT_EQ(state.current_stacks[0], 100 - 3);
  ASSERT_EQ(state.current_stacks[1], 100 - 12);
}

TEST(TestStep, RaiseAllInScenario) {
  Deck deck;
  GameState state = GameState::InitialState(6, {100, 100}, deck);

  Action bet(ActionType::Bet, 20);
  state = GameModel::Step(state, bet);
  
  Action raise(ActionType::Raise, 100);
  state = GameModel::Step(state, raise);
  
  ASSERT_EQ(state.current_stacks[1], 0);
  ASSERT_EQ(state.current_bets[1], 100);
}

TEST(TestStep, HistoryIsUpdatedWithAction) {
  Deck deck;
  GameState state = GameState::InitialState(6, {100, 100}, deck);
  Action bet(ActionType::Bet, 15);
  
  GameState next = GameModel::Step(state, bet);
  
  ASSERT_EQ(next.history.size(), 1);
  ASSERT_EQ(next.history[0].type, ActionType::Bet);
  ASSERT_EQ(next.history[0].amount, 15);
}

TEST(TestStep, HistoryAccumulatesActions) {
  Deck deck;
  GameState state = GameState::InitialState(6, {100, 100}, deck);
  state = GameModel::Step(state, Action(ActionType::Bet, 10));
  state = GameModel::Step(state, CALL);
  
  ASSERT_EQ(state.history.size(), 2);
  ASSERT_EQ(state.history[0].type, ActionType::Bet);
  ASSERT_EQ(state.history[1].type, ActionType::Call);
}

TEST(TestStep, ThrowsOnTerminalState) {
  Deck deck;
  GameState state = GameState::InitialState(6, {100, 100}, deck);
  state.is_terminal = true;
  
  ASSERT_THROW(GameModel::Step(state, CHECK), std::runtime_error);
}

TEST(TestStep, ThrowsOnIllegalAction) {
  Deck deck;
  GameState state = GameState::InitialState(6, {100, 100}, deck);
  state.current_bets = {10, 0};
  state.current_player = 1;
  
  // Check is illegal when facing a bet
  ASSERT_THROW(GameModel::Step(state, CHECK), std::runtime_error);
}
