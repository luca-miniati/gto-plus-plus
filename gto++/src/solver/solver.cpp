// #include "game/game_model.h"
// #include "solver/solver.h"
//
// double Solver::CFR(GameState state, Deck deck, double p0, double p1) {
//   if (state.IsChanceNode()) {
//     if (state.street == Street::Flop) {
//       state.community_cards = {deck.Pop(), deck.Pop(), deck.Pop()};;
//       std::sort(state.community_cards.begin(), state.community_cards.end());
//     } else {
//       state.community_cards.push_back(deck.Pop());
//     }
//   }
//
//   Node u = this->tree.GetOrCreateNode(state);
//   if (u.IsTerminal())
//     return u.GetUtility(deck);
//
//   int num_actions = u.GetNumActions();
//   double reach_p = (state.current_player == 0) ? p1 : p0;
//   double node_util = 0;
//
//   std::vector<double> util, strategy = u.GetStrategy(reach_p);
//
//   for (int a = 0; a < num_actions; ++a) {
//     GameState next = this->tree.ChildState(state, a);
//     double v;
//     if (state.current_player == 0)
//       v = -CFR(next, deck, p0 * strategy[a], p1);
//     else
//       v = -CFR(next, deck, p0, p1 * strategy[a]);
//     util.push_back(v);
//     node_util += strategy[a] * v;
//   }
//
//   for (int a = 0; a < num_actions; ++a) {
//     double regret = util[a] - node_util;
//     u.UpdateRegret(a, reach_p * regret);
//   }
//
//   return node_util;
// }
