#include <cassert>
#include <iostream>
#include <unordered_set>
#include <array>
#include <algorithm>
#include "info_set/canonical_suit_abstraction.h"

namespace {

  static inline void sort3(int* a) {
    if (a[0] < a[1]) std::swap(a[0], a[1]);
    if (a[1] < a[2]) std::swap(a[1], a[2]);
    if (a[0] < a[1]) std::swap(a[0], a[1]);
  }

  static inline void sort2(int* a) {
    if (a[0] < a[1]) std::swap(a[0], a[1]);
  }

  static constexpr int SUIT_PERMS[24][4] = {
    {0,1,2,3},{0,1,3,2},{0,2,1,3},{0,2,3,1},{0,3,1,2},{0,3,2,1},
    {1,0,2,3},{1,0,3,2},{1,2,0,3},{1,2,3,0},{1,3,0,2},{1,3,2,0},
    {2,0,1,3},{2,0,3,1},{2,1,0,3},{2,1,3,0},{2,3,0,1},{2,3,1,0},
    {3,0,1,2},{3,0,2,1},{3,1,0,2},{3,1,2,0},{3,2,0,1},{3,2,1,0}
  };

  static inline int CanonicalizeCommunity(
      const Cards& community_cards,
      std::array<int, 5>& out)
  {
    const int n = community_cards.size();

    std::array<int, 5> base{};
    for (int i = 0; i < n; ++i)
      base[i] = int(community_cards[i]);

    if (n >= 3)
      sort3(base.data());

    std::array<int, 5> best = base;

    for (int p = 0; p < 24; ++p) {
      std::array<int, 5> tmp;

      for (int i = 0; i < n; ++i) {
        int c = base[i];
        tmp[i] = c - (c % 4) + SUIT_PERMS[p][c % 4];
      }

      if (n >= 3)
        sort3(tmp.data());

      if (std::lexicographical_compare(
            tmp.begin(), tmp.begin() + n,
            best.begin(), best.begin() + n))
        best = tmp;
    }

    out = best;
    return n;
  }

  static inline int CanonicalizeAll(
      const Cards& community_cards,
      const Cards& hole_cards,
      std::array<int, 7>& out)
  {
    const int nC = community_cards.size();
    const int total = nC + 2;

    std::array<int, 7> base{};

    for (int i = 0; i < nC; ++i)
      base[i] = int(community_cards[i]);

    for (int i = 0; i < 2; ++i)
      base[nC + i] = int(hole_cards[i]);

    if (nC >= 3)
      sort3(base.data());

    sort2(base.data() + nC);

    std::array<int, 7> best = base;

    for (int p = 0; p < 24; ++p) {
      std::array<int, 7> tmp;

      for (int i = 0; i < total; ++i) {
        int c = base[i];
        tmp[i] = c - (c % 4) + SUIT_PERMS[p][c % 4];
      }

      if (nC >= 3)
        sort3(tmp.data());

      sort2(tmp.data() + nC);

      if (std::lexicographical_compare(
            tmp.begin(), tmp.begin() + total,
            best.begin(), best.begin() + total))
        best = tmp;
    }

    out = best;
    return total;
  }

  static inline uint64_t EncodeSuits(const int* cards, int n) {
    uint64_t suits = 0;
    for (int i = 0; i < n; ++i)
      suits = (suits << 3) | (cards[i] % 4 + 1);
    return suits;
  }

  static inline uint64_t EncodeRanks(const int* cards, int n) {
    uint64_t ranks = 0;
    for (int i = 0; i < n; ++i)
      ranks = (ranks << 4) | (cards[i] / 4 + 1);
    return ranks;
  }

  static inline uint64_t EncodeActions(const std::vector<int>& history) {
    uint64_t actions = 0;
    for (int a : history)
      actions = (actions << 3) | (a + 1);
    return actions;
  }

} // namespace

BoardKey CanonicalSuitAbstraction::GetBoardKey(const Cards &community_cards) const {
  Cards board = community_cards;
  std::sort(board.begin(), board.end());
  std::array<int, 5> canonical{};
  int n = CanonicalizeCommunity(community_cards, canonical);

  uint64_t suits = EncodeSuits(canonical.data(), n);
  uint64_t ranks = EncodeRanks(canonical.data(), n);

  PublicInfoKey key = suits;
  key = (key << 20) | ranks;
  key = (key << 40) | actions;
  return key;
}

PublicInfoKey CanonicalSuitAbstraction::GetPublicInfoKey(
    const Cards &community_cards,
    const std::vector<int> &history) const {

  std::array<int, 5> canonical{};
  int n = CanonicalizeCommunity(community_cards, canonical);

  uint64_t suits = EncodeSuits(canonical.data(), n);
  uint64_t ranks = EncodeRanks(canonical.data(), n);
  uint64_t actions = EncodeActions(history);

  PublicInfoKey key = suits;
  key = (key << 20) | ranks;
  key = (key << 40) | actions;
  return key;
}

PrivateInfoKey CanonicalSuitAbstraction::GetPrivateInfoKey(
    const Cards &community_cards,
    const std::vector<Card> &hole_cards) const {

  std::array<int, 7> canonical{};
  int total = CanonicalizeAll(
      community_cards,
      hole_cards,
      canonical);

  const int nC = community_cards.size();

  std::array<int, 2> hole{};
  hole[0] = canonical[nC];
  hole[1] = canonical[nC+1];

  uint64_t suits = (hole[0] % 4 + 1);
  suits = (suits << 3) | (hole[1] % 4 + 1);

  uint64_t ranks = (hole[0] / 4 + 1);
  ranks = (ranks << 4) | (hole[1] / 4 + 1);

  return (suits << 8) | ranks;
}

std::map<PrivateInfoKey, std::vector<Cards>> CanonicalSuitAbstraction::GetHandsByPrivateInfoKey(
    const Cards &community_cards) const
{
  std::map<PrivateInfoKey, std::vector<Cards>> ans;

  for (Card c1 : CARDS) {
    for (Card c2 : CARDS) {
      if (c1 > c2 &&
          !std::ranges::contains(community_cards, c1) &&
          !std::ranges::contains(community_cards, c2)) {
        PrivateInfoKey key = GetPrivateInfoKey(community_cards, {c1, c2});
        if (ans.count(key))
          ans[key].emplace_back(c1, c2);
        else
          ans[key] = {{c1, c2}};
      }
    }
  }

  return ans;
}
