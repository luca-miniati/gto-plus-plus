#include <cassert>
#include <iostream>
#include <unordered_set>
#include "info_set/canonical_suit_abstraction.h"

/*
 * Sorts the flop, returns cards as ints.
 */
std::vector<int> CommunityCardsToInts(const Cards &community_cards) {
  std::vector<int> cards;
  for (int i = 0; i < 3; ++i)
    cards.push_back(int(community_cards[i]));
  std::ranges::sort(cards, std::ranges::greater());
  for (int i = 3; i < community_cards.size(); ++i)
    cards.push_back(int(community_cards[i]));
  return cards;
}

std::vector<int> Canonicalize(
    const Cards &community_cards
    ) {
  std::vector<int> ccs;
  for (Card c : community_cards)
    ccs.push_back(int(c));
  std::sort(ccs.begin(), ccs.begin() + 3);  // sort flop

  std::vector<int> iso = {0, 1, 2, 3};
  std::vector<int> best = ccs;

  do {
    std::vector<int> cards;
    for (int c : ccs)
      // reassign the suit
      cards.push_back(c - c % 4 + iso[c % 4]);
    std::sort(cards.begin(), cards.begin() + 3);
    best = std::min(best, cards);
  } while (std::next_permutation(iso.begin(), iso.end()));

  return best;
}

std::vector<int> Canonicalize(
    const Cards &community_cards,
    const Cards &hole_cards
    ) {
  std::vector<int> ccs;
  for (Card c : community_cards)
    ccs.push_back(int(c));
  std::sort(ccs.begin(), ccs.begin() + 3);  // sort flop
  std::vector<int> hcs;
  for (Card c : hole_cards)
    hcs.push_back(int(c));
  std::sort(hcs.begin(), hcs.end());  // sort hole cards

  std::vector<int> iso = {0, 1, 2, 3};
  std::vector<int> best;
  for (int c : ccs) best.push_back(c);
  for (int c : hcs) best.push_back(c);
  std::sort(best.begin(), best.begin() + 3);
  std::sort(best.begin() + ccs.size(),
      best.begin() + ccs.size() + 2);

  do {
    std::vector<int> out;
    for (int c : ccs)
      // reassign the suit
      out.push_back(c - c % 4 + iso[c % 4]);
    for (int c : hcs)
      // reassign the suit
      out.push_back(c - c % 4 + iso[c % 4]);
    std::sort(out.begin(), out.begin() + 3);
    std::sort(out.begin() + ccs.size(),
        out.begin() + ccs.size() + 2);
    best = std::min(best, out);
  } while (std::next_permutation(iso.begin(), iso.end()));

  return best;

}

uint64_t EncodeSuits(const std::vector<int> &cards) {
  uint64_t suits = 0;
  for (int c : cards)
    suits = (suits << 3) | (c % 4 + 1);
  return suits;
}

uint64_t EncodeRanks(const std::vector<int> &cards) {
  uint64_t ranks = 0;
  for (int c : cards)
    ranks = (ranks << 4) | (c / 4 + 1);
  return ranks;
}

uint64_t EncodeActions(const std::vector<int> &history) {
  uint64_t actions = 0;
  for (int a : history)
    actions = (actions << 3) | (a + 1);
  return actions;
}

PublicInfoKey CanonicalSuitAbstraction::GetPublicInfoKey(
    const Cards &community_cards,
    const std::vector<int> &history) const {

  std::vector<int> ccs = Canonicalize(community_cards);

  uint64_t suits = EncodeSuits(ccs);
  uint64_t ranks = EncodeRanks(ccs);
  uint64_t actions = EncodeActions(history);

  PublicInfoKey key;
  key = suits;
  key = (key << 20) | ranks;
  key = (key << 40) | actions;
  return key;
}

PrivateInfoKey CanonicalSuitAbstraction::GetPrivateInfoKey(
    const Cards &community_cards,
    const std::vector<Card> &hole_cards) const {

  std::vector<int> canonicalized = Canonicalize(community_cards, hole_cards);

  std::vector<int> hcs;
  for (int i = community_cards.size(); i < community_cards.size() + 2; ++i)
    hcs.push_back(canonicalized[i]);

  uint64_t hole_suits = EncodeSuits(hcs);
  uint64_t ranks = EncodeRanks(hcs);

  return (hole_suits << 8) | ranks;
}

std::vector<PrivateInfoKey> CanonicalSuitAbstraction::GetAllPrivateInfoKeys(
    const Cards &community_cards) const {
  std::unordered_set<PrivateInfoKey> keys;
  for (Card c1 : CARDS)
    for (Card c2 : CARDS)
      if (c1 != c2 &&
          !std::ranges::contains(community_cards, c1) &&
          !std::ranges::contains(community_cards, c2))
        keys.insert(GetPrivateInfoKey(community_cards, {c1, c2}));
  std::vector<PrivateInfoKey> ans(keys.begin(), keys.end());
  std::ranges::sort(ans);
  return ans;
}

