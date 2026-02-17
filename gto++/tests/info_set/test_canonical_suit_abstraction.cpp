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
