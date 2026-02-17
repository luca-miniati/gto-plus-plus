#include <cassert>
#include <iostream>
#include "info_set/canonical_suit_abstraction.h"


/*
 * Given a sequence of suits src, return the canonical sequence of tgt with
 * respect to src.
 */
uint64_t Canonicalize(const uint64_t &tgt, int num_cards_tgt,
    const uint64_t &src, int num_cards_src) {
  // iso[s] = -1 => suit s hasn't been bound
  std::array<int, 5> iso = {-1, -1, -1, -1, -1};

  int mn_unused_suit = 1;
  for (int i = num_cards_src - 1; i >= 0; --i) {
    int s = (src >> 3 * i) & 0b111;
    if (iso[s] == -1) iso[s] = mn_unused_suit++;
  }

  uint64_t ans = 0;
  for (int i = num_cards_tgt - 1; i >= 0; --i) {
    int s = (tgt >> 3 * i) & 0b111;
    if (iso[s] == -1) iso[s] = mn_unused_suit++;
    ans |= (iso[s] << 3 * i);
  }

  return ans;
}

PublicInfoKey CanonicalSuitAbstraction::GetPublicInfoKey(
    const Cards &community_cards,
    const std::vector<int> &history) const {
  std::vector<int> cards;
  for (int i = 0; i < 3; ++i)
    cards.push_back(int(community_cards[i]));
  std::ranges::sort(cards, std::ranges::greater());
  for (int i = 3; i < community_cards.size(); ++i)
    cards.push_back(int(community_cards[i]));

  PublicInfoKey suits = 0;  // <= 3 * 5 = 15 bits
  for (int c : cards)
    suits = (suits << 3) | (c % 4 + 1);
  suits = Canonicalize(suits, cards.size(), suits, cards.size());

  PublicInfoKey ranks = 0;  // <= 4 * 5 = 20 bits
  for (int c : cards)
    ranks = (ranks << 4) | (c / 4 + 1);

  PublicInfoKey actions = 0;
  for (int a : history)
    actions = (actions << 3) | (a + 1);

  return (((suits << 20) | ranks) << 40) | actions;
}

PrivateInfoKey CanonicalSuitAbstraction::GetPrivateInfoKey(
    const Cards &community_cards,
    const std::vector<Card> &hole_cards) const {
  int num_community_cards = community_cards.size();
  std::vector<int> cards;
  for (int i = 0; i < 3; ++i)
    cards.push_back(int(community_cards[i]));
  std::ranges::sort(cards, std::ranges::greater());
  for (int i = 3; i < num_community_cards; ++i)
    cards.push_back(int(community_cards[i]));

  uint64_t board_suits = 0; 
  for (int c : cards)
    board_suits = (board_suits << 3) | (c % 4 + 1);
  board_suits = Canonicalize(board_suits, num_community_cards, board_suits,
      num_community_cards);

  int c1 = std::min(int(hole_cards[0]), int(hole_cards[1]));
  int c2 = std::max(int(hole_cards[0]), int(hole_cards[1]));
  cards.push_back(c1);
  cards.push_back(c2);

  uint64_t hole_suits = ((c1 % 4 + 1) << 3) | (c2 % 4 + 1);
  hole_suits = Canonicalize(hole_suits, 2, board_suits, num_community_cards);

  uint64_t ranks = ((c1 / 4 + 1) << 4) | (c2 / 4 + 1); 

  return (hole_suits << 8) | ranks;
}
