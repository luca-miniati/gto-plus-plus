#include <algorithm>
#include <random>
#include <iostream>
#include "game/game_model.h"
#include "utils/utils.h"
#include "solver/solver.h"

Solver::Solver(
    std::vector<Chips> pot_contributions,
    int max_raises,
    std::vector<Chips> starting_stacks,
    std::unique_ptr<ActionAbstraction> action_abst,
    std::unique_ptr<InfoSetAbstraction> info_set_abst,
    Cards flop)
  : tree_(
      std::move(pot_contributions), 
      max_raises, 
      std::move(starting_stacks), 
      std::move(action_abst), 
      std::move(info_set_abst), 
      flop
      ),
    flop_(flop)
{
  tree_.Build();
}

int randint(int lo, int hi) {
  static std::random_device rd;
  static std::mt19937 gen(rd());
  std::uniform_int_distribution<> dist(lo, hi);
  return dist(gen);
}

Card DealCard(Cards &deck) {
  Card c = deck.back();
  deck.pop_back();
  return c;
}

double Solver::CFR(NodePtr u, std::vector<int> history, Cards community_cards,
    Cards p0_cards, Cards p1_cards, Cards deck, double p0, double p1) {

  if (u->IsTerminal()) {
    return GameModel::GetUtility(
        /* state            = */ u->GetTerminalGameState(),
        /* community_cards  = */ community_cards,
        /* p0_cards         = */ p0_cards,
        /* p1_cards         = */ p1_cards
        );
  }

  if (u->IsChanceNode()) {
    // sample a raw card
    community_cards.push_back(DealCard(deck));

    // make the PublicInfoKey for this new GameState
    PublicInfoKey key = tree_.GetPublicInfoKey(community_cards, history);

    // lookup the node for that PublicStateKey
    return CFR(
        /* u                = */ tree_.GetNode(key),
        /* history          = */ history,
        /* community_cards  = */ community_cards,
        /* p0_cards         = */ p0_cards,
        /* p1_cards         = */ p1_cards,
        /* deck             = */ deck,
        /* p0               = */ p0,
        /* p1               = */ p1
       );
  }

  int current_player = u->GetCurrentPlayer();
  PrivateInfoKey key = tree_.GetPrivateInfoKey(
      community_cards,
      (current_player == 0) ? p0_cards : p1_cards
      );

  if (!u->HasStrategy(key))
    u->InitStrategy(key);

  double reach_p_self = (current_player == 0) ? p0 : p1;
  double reach_p_opp = (current_player == 0) ? p1 : p0;
  u->UpdateStrategy(key, reach_p_self);

  double node_util = 0;
  int num_actions = u->GetNumActions();
  std::vector<double> util(num_actions);

  for (int action_idx = 0; action_idx < num_actions; ++action_idx) {
    // do action
    history.push_back(action_idx);
    double action_p = u->GetStrategy(key, action_idx);

    double ev = CFR(
        /* u                = */ tree_.GetChild(u, action_idx),
        /* history          = */ history,
        /* community_cards  = */ community_cards,
        /* p0_cards         = */ p0_cards,
        /* p1_cards         = */ p1_cards,
        /* deck             = */ deck,
        /* p0               = */ (current_player == 0) ? p0 * action_p : p0,
        /* p1               = */ (current_player == 1) ? p1 * action_p : p1
        );

    util[action_idx] = ev;
    node_util += action_p * ev;

    // backtrack
    history.pop_back();
  }

  for (int action_idx = 0; action_idx < num_actions; ++action_idx) {
    double regret = util[action_idx] - node_util;
    u->UpdateRegret(key, action_idx, reach_p_opp * regret);
  }

  return node_util;
}

void Solver::Solve() {
  NodePtr root = tree_.GetRoot();

  Cards deck = MakeDeck();
  std::erase(deck, flop_[0]);
  std::erase(deck, flop_[1]);
  std::erase(deck, flop_[2]);

  double total = 0;
  for (int i = 0; i < 500000; ++i) {
    ShuffleDeck(deck);

    // deal a hand
    Cards p0_cards = {DealCard(deck), DealCard(deck)};
    Cards p1_cards = {DealCard(deck), DealCard(deck)};

    double ev = CFR(
        /* u                = */ root,
        /* history          = */ {},
        /* community_cards  = */ flop_,
        /* p0_cards         = */ p0_cards,
        /* p1_cards         = */ p1_cards,
        /* deck             = */ deck,
        /* p0               = */ 1,
        /* p1               = */ 1
        );
    
    // restore hole cards to deck for next iter
    deck.push_back(p0_cards[0]);
    deck.push_back(p0_cards[1]);
    deck.push_back(p1_cards[0]);
    deck.push_back(p1_cards[1]);

    if (i > 5000)
      total += ev;
    if (i % 10000 == 1)
      std::cout << total / i << std::endl;
  }
}
