#include <cassert>
#include <stdexcept>
#include "action/action.h"
#include "game/game_model.h"
#include "game/game_state.h"

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
  if (state.is_terminal)
    throw std::runtime_error("can't call Step on terminal game state");

  if (!IsLegal(state, action))
    throw std::runtime_error("illegal action");

  int actor = state.current_player;
  int opponent = actor ^ 1;
  int b0 = state.current_bets[actor];
  int b1 = state.current_bets[actor ^ 1];
  int s0 = state.current_stacks[actor];
  int s1 = state.current_stacks[actor ^ 1];
  int effective_stack = std::min(s0, s1);

  GameState next_state;
  auto next_deck            = state.deck;
  auto next_community_cards = state.community_cards;
  auto next_stacks          = state.current_stacks;
  auto next_bets            = state.current_bets;
  auto next_history         = state.history;
  next_history.push_back(action);
  int next_pot = state.pot;

  switch (action.type) {
    case ActionType::Bet:
      {
        // Actor places a bet
        next_stacks[actor] -= action.amount;
        next_bets[actor] = action.amount;
        next_state = GameState{
            /*is_terminal         =*/ false,
            /*current_player      =*/ opponent,
            /*current_raises      =*/ 1,
            /*pot                 =*/ next_pot,
            /*street              =*/ state.street,
            /*deck                =*/ next_deck,
            /*community_cards     =*/ next_community_cards,
            /*current_stacks      =*/ next_stacks,
            /*current_bets        =*/ next_bets,
            /*history             =*/ next_history
        };
        break;
      }
    case ActionType::Call:
      {
        // Actor calls the bet
        int call_amount = std::abs(b1 - b0);
        next_bets[actor] += call_amount;
        assert(next_bets[0] == next_bets[1]);
        next_stacks[actor] -= call_amount;
        next_pot += 2 * next_bets[0];
        next_bets.assign(2, 0);

        bool is_river = state.street == Street::River;

        // If not river, deal next card
        if (!is_river)
          next_community_cards.push_back(next_deck.Pop());

        next_state = GameState{
            /*is_terminal         =*/ is_river,
            /*current_player      =*/ opponent,
            /*current_raises      =*/ 0,
            /*pot                 =*/ next_pot,
            /*street              =*/ is_river ? state.street : NextStreet(state.street),
            /*deck                =*/ next_deck,
            /*community_cards     =*/ next_community_cards,
            /*current_stacks      =*/ next_stacks,
            /*current_bets        =*/ next_bets,
            /*history             =*/ next_history
        };
        break;
      }
    case ActionType::Check:
      {
        // Check - action passes to opponent or street advances
        bool opponent_has_checked = (state.history.size() > 0 && 
            state.history.back().type == ActionType::Check);

        if (opponent_has_checked) {
          // Both players checked, advance street
          next_bets.assign(2, 0);
          bool is_river = state.street == Street::River;

          // If not river, deal next card
          if (!is_river)
            next_community_cards.push_back(next_deck.Pop());

          next_state = GameState{
              /*is_terminal         =*/ is_river,
              /*current_player      =*/ opponent,
              /*current_raises      =*/ 0,
              /*pot                 =*/ next_pot,
              /*street              =*/ is_river ? state.street : NextStreet(state.street),
              /*deck                =*/ next_deck,
              /*community_cards     =*/ next_community_cards,
              /*current_stacks      =*/ next_stacks,
              /*current_bets        =*/ next_bets,
              /*history             =*/ next_history
          };
        } else {
          // First check, pass to opponent
          next_state = GameState{
              /*is_terminal         =*/ false,
              /*current_player      =*/ opponent,
              /*current_raises      =*/ state.current_raises,
              /*pot                 =*/ next_pot,
              /*street              =*/ state.street,
              /*deck                =*/ next_deck,
              /*community_cards     =*/ next_community_cards,
              /*current_stacks      =*/ next_stacks,
              /*current_bets        =*/ next_bets,
              /*history             =*/ next_history
          };
        }
        break;
      }
    case ActionType::Fold:
      {
        // Player folds, game is terminal
        next_state = GameState{
            /*is_terminal         =*/ true,
            /*current_player      =*/ actor,
            /*current_raises      =*/ state.current_raises,
            /*pot                 =*/ next_pot,
            /*street              =*/ state.street,
            /*deck                =*/ next_deck,
            /*community_cards     =*/ next_community_cards,
            /*current_stacks      =*/ next_stacks,
            /*current_bets        =*/ next_bets,
            /*history             =*/ next_history
        };
        break;
      }
    case ActionType::Raise:
      {
        int raise_difference = action.amount - state.current_bets[actor];
        next_stacks[actor] -= raise_difference;
        next_bets[actor] = action.amount;

        next_state = GameState{
            /*is_terminal         =*/ false,
            /*current_player      =*/ opponent,
            /*current_raises      =*/ state.current_raises + 1,
            /*pot                 =*/ next_pot,
            /*street              =*/ state.street,
            /*deck                =*/ next_deck,
            /*community_cards     =*/ next_community_cards,
            /*current_stacks      =*/ next_stacks,
            /*current_bets        =*/ next_bets,
            /*history             =*/ next_history
        };
        break;
      }
  }

  return next_state;
}
