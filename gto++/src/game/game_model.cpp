#include "game/game_model.h"
#include "action/action.h"

bool GameModel::IsLegal(const GameState &state, const Action &action) {
  int actor = state.current_player;
  // WLOG, it's p0 to move. Cursed? Yes
  int b0 = state.current_bets[actor];
  int b1 = state.current_bets[actor ^ 1];
  int s0 = state.current_stacks[actor];
  int s1 = state.current_stacks[actor ^ 1];
  int effective_stack = std::min(s0, s1);

  // prune illegal actions
  switch (action.type) {
    case ActionType::Bet:
      if (b0 != 0 || b1 != 0)
        return false;  // bet is only legal if there's not already a bet

      if (action.amount < 2 || action.amount > effective_stack)  // min bet is 2 chips (1bb)
        return false;

      break;

    case ActionType::Call:
      if (b0 == b1)
        return false;  // can't call when there's no bet

      if (b0 > b1)
        return false;  // can't call when if you made the bet yourself
                       // this path shouldn't get called anyways
      break;

    case ActionType::Check:
      if (b0 != b1)
        return false;  // can't check when there's a bet

      break;

    case ActionType::Fold:
      if (b0 == b1)
        return false;  // can't fold when there's no bet

      break;

    case ActionType::Raise:
      if (b0 == b1)
        return false;

      int last_raise = abs(b0 - b1);  // this breaks for > 2 players, but the
                                      // diff in current bets is always the
                                      // last raise in HU
      int raise_difference = action.amount - std::max(b0, b1);
      int bettor_total_chips = b0 + s0;

      if (action.amount > bettor_total_chips)
        return false;  // check that bettor has enough chips to make raise

      if (action.amount < bettor_total_chips && raise_difference < last_raise)
        return false;  // needs to increase by at least the last raise, if it's
                       // not an all-in

      break;
  }

  return true;
}

GameState GameModel::Step(const GameState &state, const Action &action) {
  return {};
}
