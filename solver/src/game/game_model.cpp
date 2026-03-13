#include <cassert>
#include <stdexcept>
#include "action/action.h"
#include "game/game_model.h"
#include "game/game_state.h"
using Rank = phevaluator::Rank;

bool GameModel::IsLegal(const GameState &state, const Action &action, int max_raises) {
  int actor = state.current_player;
  // WLOG, it's p0 to move. Cursed? Yes
  Chips b0 = state.current_bets[actor];
  Chips b1 = state.current_bets[actor ^ 1];
  Chips s0 = state.current_stacks[actor];
  Chips s1 = state.current_stacks[actor ^ 1];
  Chips effective_stack = std::min(s0, s1);

  // prune illegal actions
  switch (action.type) {
    case ActionType::Bet:
      if (b0 != 0 || b1 != 0)
        return false;  // bet is only legal if there's not already a bet

      if (action.amount < 2)  // min bet is 2 chips (1bb)
        throw std::runtime_error("GameModel.Step: invalid action: betting < 1bb");

      if (action.amount > effective_stack)
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
      if (state.current_raises >= max_raises)
        return false;

      if (b0 == b1)
        return false;

      Chips last_raise = abs(b0 - b1);  // this breaks for > 2 players, but the
                                      // diff in current bets is always the
                                      // last raise in HU
      Chips raise_difference = action.amount - std::max(b0, b1);
      Chips bettor_total_chips = b0 + s0;

      if (action.amount > bettor_total_chips)
        return false;  // check that bettor has enough chips to make raise

      if (action.amount < bettor_total_chips && raise_difference < last_raise)
        return false;  // needs to increase by at least the last raise, if it's
                       // not an all-in

      break;
  }

  return true;
}

GameState GameModel::Step(const GameState &state, const Card &card) {
  if (!state.IsChance())
    throw std::runtime_error("GameModel::Step: tried to deal card to non-chance node");

  GameState next_state = state;
  next_state.community_cards.push_back(card);
  return next_state;
}

GameState GameModel::Step(const GameState &state, const Action &action,
    const int action_idx) {
  if (state.is_terminal)
    throw std::runtime_error("can't call Step on terminal game state");

  int actor = state.current_player;
  int opponent = actor ^ 1;
  Chips b0 = state.current_bets[actor];
  Chips b1 = state.current_bets[actor ^ 1];
  Chips s0 = state.current_stacks[actor];
  Chips s1 = state.current_stacks[actor ^ 1];
  Chips effective_stack = std::min(s0, s1);

  auto next_stacks          = state.current_stacks;
  auto next_bets            = state.current_bets;
  auto next_history         = state.history;
  next_history.push_back(action_idx);
  auto next_pot_contributions = state.pot_contributions;

  switch (action.type) {
    case ActionType::Bet:
      {
        // Actor places a bet
        next_stacks[actor] -= action.amount;
        next_bets[actor] = action.amount;
        return GameState{
            /*is_terminal         =*/ false,
            /*current_player      =*/ opponent,
            /*current_raises      =*/ 1,
            /*street              =*/ state.street,
            /*community_cards     =*/ state.community_cards,
            /*current_stacks      =*/ next_stacks,
            /*current_bets        =*/ next_bets,
            /*pot_contributions   =*/ next_pot_contributions,
            /*history             =*/ next_history
        };
      }
    case ActionType::Call:
      {
        // Actor calls the bet
        Chips call_amount = std::abs(b1 - b0);
        next_bets[actor] += call_amount;
        assert(next_bets[0] == next_bets[1]);
        next_stacks[actor] -= call_amount;
        next_pot_contributions[0] += next_bets[0];
        next_pot_contributions[1] += next_bets[1];
        next_bets = {0, 0};

        bool is_river = state.street == Street::River;

        return GameState{
            /*is_terminal         =*/ is_river,
            /*current_player      =*/ 0,
            /*current_raises      =*/ 0,
            /*street              =*/ is_river ? state.street : NextStreet(state.street),
            /*community_cards     =*/ state.community_cards,
            /*current_stacks      =*/ next_stacks,
            /*current_bets        =*/ next_bets,
            /*pot_contributions   =*/ next_pot_contributions,
            /*history             =*/ next_history
        };
      }
    case ActionType::Check:
      {
        // Check - action passes to opponent or street advances
        bool opponent_has_checked = (actor == 1 && state.history.size() > 0 && 
            b0 == 0 && b1 == 0);

        if (opponent_has_checked) {
          // Both players checked, advance street
          next_bets = {0, 0};
          bool is_river = state.street == Street::River;

          return GameState{
              /*is_terminal         =*/ is_river,
              /*current_player      =*/ 0,
              /*current_raises      =*/ 0,
              /*street              =*/ is_river ? state.street : NextStreet(state.street),
              /*community_cards     =*/ state.community_cards,
              /*current_stacks      =*/ next_stacks,
              /*current_bets        =*/ next_bets,
              /*pot_contributions   =*/ next_pot_contributions,
              /*history             =*/ next_history
          };
        } else {
          // First check, pass to opponent
          return GameState{
              /*is_terminal         =*/ false,
              /*current_player      =*/ opponent,
              /*current_raises      =*/ 0,
              /*street              =*/ state.street,
              /*community_cards     =*/ state.community_cards,
              /*current_stacks      =*/ next_stacks,
              /*current_bets        =*/ next_bets,
              /*pot_contributions   =*/ next_pot_contributions,
              /*history             =*/ next_history
          };
        }
      }
    case ActionType::Fold:
      {
        // Player folds, game is terminal
        return GameState{
            /*is_terminal         =*/ true,
            /*current_player      =*/ actor,
            /*current_raises      =*/ state.current_raises,
            /*street              =*/ state.street,
            /*community_cards     =*/ state.community_cards,
            /*current_stacks      =*/ next_stacks,
            /*current_bets        =*/ next_bets,
            /*pot_contributions   =*/ next_pot_contributions,
            /*history             =*/ next_history
        };
      }
    case ActionType::Raise:
      {
        Chips raise_difference = action.amount - state.current_bets[actor];
        next_stacks[actor] -= raise_difference;
        next_bets[actor] = action.amount;

        return GameState{
            /*is_terminal         =*/ false,
            /*current_player      =*/ opponent,
            /*current_raises      =*/ state.current_raises + 1,
            /*street              =*/ state.street,
            /*community_cards     =*/ state.community_cards,
            /*current_stacks      =*/ next_stacks,
            /*current_bets        =*/ next_bets,
            /*pot_contributions   =*/ next_pot_contributions,
            /*history             =*/ next_history
        };
      }
    default:
      throw std::runtime_error("GameModel::Step: invalid ActionType");
  }
}

/*
 * Return the utility of a given game state.
 */
double GameModel::GetUtility(const GameState &state,
                             const Cards &p0_cards,
                             const Cards &p1_cards)
{
  if (state.history.size() == 0)
      throw std::runtime_error("GameModel::GetUtility: called on state with empty history.");
  if (!state.is_terminal)
      throw std::runtime_error("GameModel::GetUtility: called on non-terminal state.");

  Chips b0 = state.current_bets[0];
  Chips b1 = state.current_bets[1];
  Chips c0 = state.pot_contributions[0];
  Chips c1 = state.pot_contributions[1];
  if (b0 < b1)  // this means p0 folded to a bet
    return -(c0 + b0);
  else if (b1 < b0)  // p1 folded to a bet
    return c1 + b1;

  Rank r0 = phevaluator::EvaluateCards(
      p0_cards[0], p0_cards[1],  // p0's hole cards
      state.community_cards[0], state.community_cards[1], state.community_cards[2],
      state.community_cards[3], state.community_cards[4]  // board
      );
  Rank r1 = phevaluator::EvaluateCards(
      p1_cards[0], p1_cards[1],  // p1's hole cards
      state.community_cards[0], state.community_cards[1], state.community_cards[2],
      state.community_cards[3], state.community_cards[4]  // board
      );

  if (r0 < r1)  // winning hand has smaller rank
    return c1;
  else if (r1 < r0)
    return -c0;
  return 0.0;  // chop chop
}
