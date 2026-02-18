#include <gtest/gtest.h>
#include "info_set/info_set_abstraction.h"
#include "info_set/canonical_suit_abstraction.h"
#include "utils/utils.h"

namespace {
  const CanonicalSuitAbstraction abst;
}

TEST(TestPublicInfoKey, TestSuitAbstraction) {
  std::vector<int> history = {};

  Cards cards;
  PublicInfoKey key, suits, ranks;

  cards = {Card("As"), Card("Kd"), Card("Jh")};
  key = abst.GetPublicInfoKey(cards, history);
  suits = 0b001'010'011;
  ranks = 0b1101'1100'1010;
  ASSERT_EQ(key >> 60, suits);
  ASSERT_EQ((key >> 40) & 0b1111'1111'1111, ranks);
  ASSERT_EQ(key, ((suits << 20) | ranks) << 40);

  cards = {Card("As"), Card("Kd"), Card("Jh"), Card("4c")};
  key = abst.GetPublicInfoKey(cards, history);
  suits = 0b001'010'011'100;
  ranks = 0b1101'1100'1010'0011;
  ASSERT_EQ(key >> 60, suits);
  ASSERT_EQ((key >> 40) & 0b1111'1111'1111'1111, ranks);
  ASSERT_EQ(key, ((suits << 20) | ranks) << 40);

  cards = {Card("As"), Card("Kd"), Card("Jh"), Card("4h")};
  key = abst.GetPublicInfoKey(cards, history);
  suits = 0b001'010'011'011;
  ranks = 0b1101'1100'1010'0011;
  ASSERT_EQ(key >> 60, suits);
  ASSERT_EQ((key >> 40) & 0b1111'1111'1111'1111, ranks);
  ASSERT_EQ(key, ((suits << 20) | ranks) << 40);

  cards = {Card("As"), Card("Kd"), Card("Jh"), Card("4s")};
  key = abst.GetPublicInfoKey(cards, history);
  suits = 0b001'010'011'001;
  ranks = 0b1101'1100'1010'0011;
  ASSERT_EQ(key >> 60, suits);
  ASSERT_EQ((key >> 40) & 0b1111'1111'1111'1111, ranks);
  ASSERT_EQ(key, ((suits << 20) | ranks) << 40);

  cards = {Card("As"), Card("Kd"), Card("Jh"), Card("4d")};
  key = abst.GetPublicInfoKey(cards, history);
  suits = 0b001'010'011'010;
  ranks = 0b1101'1100'1010'0011;
  ASSERT_EQ(key >> 60, suits);
  ASSERT_EQ((key >> 40) & 0b1111'1111'1111'1111, ranks);
  ASSERT_EQ(key, ((suits << 20) | ranks) << 40);
}

TEST(TestPrivateInfoKey, TestSuitAbstraction) {
  Cards board, cards;
  PublicInfoKey key, suits, ranks;

  board = {Card("Ad"), Card("Kd"), Card("Qd")};
  cards = {Card("Jc"), Card("Tc")};
  key = abst.GetPrivateInfoKey(board, cards);
  suits = 0b010'010;
  ranks = 0b1010'1001;
  ASSERT_EQ(key >> 8, suits);
  ASSERT_EQ(key & 0b1111'1111, ranks);
  ASSERT_EQ(key, (suits << 8) | ranks);

  board = {Card("Ad"), Card("Kd"), Card("Qd")};
  cards = {Card("Jh"), Card("Th")};
  key = abst.GetPrivateInfoKey(board, cards);
  suits = 0b010'010;
  ranks = 0b1010'1001;
  ASSERT_EQ(key >> 8, suits);
  ASSERT_EQ(key & 0b1111'1111, ranks);
  ASSERT_EQ(key, (suits << 8) | ranks);

  board = {Card("Ad"), Card("Kd"), Card("Qd")};
  cards = {Card("Js"), Card("Ts")};
  key = abst.GetPrivateInfoKey(board, cards);
  suits = 0b010'010;
  ranks = 0b1010'1001;
  ASSERT_EQ(key >> 8, suits);
  ASSERT_EQ(key & 0b1111'1111, ranks);
  ASSERT_EQ(key, (suits << 8) | ranks);

  board = {Card("Ad"), Card("Kd"), Card("Qd")};
  cards = {Card("Jd"), Card("Td")};
  key = abst.GetPrivateInfoKey(board, cards);
  suits = 0b001'001;
  ranks = 0b1010'1001;
  ASSERT_EQ(key >> 8, suits);
  ASSERT_EQ(key & 0b1111'1111, ranks);
  ASSERT_EQ(key, (suits << 8) | ranks);
}

TEST(TestPrivateInfoKey, TestGetAllPrivateInfoKeys) {
  Cards board, cards;
  PublicInfoKey key, suits, ranks;

  board = {Card("Ad"), Card("Kd"), Card("Qd")};

  std::vector<PrivateInfoKey> keys = abst.GetAllPrivateInfoKeys(board);

  // flushes: choose 2 of {2d, 3d, ..., Jd} = C(10, 2) = 45
  int flushes = 0;
  for (PrivateInfoKey k : keys)
    if ((k >> 8) == 0b001'001)
      ++flushes;
  ASSERT_EQ(flushes, 45);

  // 0 d:
  //   1 combo of each PP: 13
  //   1 combo of each offsuit (both not diamonds): C(13, 2) = 78
  //   1 combo of each suited (not diamonds): C(13, 2) = 78
  int zero_diamond = 0;
  for (PrivateInfoKey k : keys)
    if ((k >> 8) != 0b001'010 &&
        (k >> 8) != 0b010'001 &&
        (k >> 8) != 0b001'001)
      ++zero_diamond;
  ASSERT_EQ(zero_diamond, 13 + 78 + 78);

  // 1 d:
  //   diamond comes from {2d, 3d, ..., Jd}
  //   non-diamond comes from {2, 3, ..., A}
  //   = 10 * 13
  int one_diamond = 0;
  for (PrivateInfoKey k : keys)
    if ((k >> 8) == 0b001'010 || (k >> 8) == 0b010'001)
      ++one_diamond;
  ASSERT_EQ(one_diamond, 130);

  ASSERT_EQ(keys.size(), 45 + 13 + 78 + 78 + 130);
}
