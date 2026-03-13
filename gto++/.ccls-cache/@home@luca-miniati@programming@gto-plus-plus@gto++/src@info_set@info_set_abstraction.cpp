#include <cassert>
#include "info_set/info_set_abstraction.h"

/*
 * Return an 8-bit representation of a hole card combo.
 * Assumes that rank1 >= rank2
 */
void EncodeHoleCards(uint8_t &hole_cards, int rank1, int rank2) {
  hole_cards = (rank1 << 4) | rank2;
}

/*
 * Return a 20-bit representation of community cards.
 * Flop is sorted. Turn and river kept in original order.
 */
void EncodeBoardRanks(uint32_t &community_cards,
                          const std::vector<Card>& cards) {
  community_cards = 0;

  int n = cards.size();
  assert(n >= 3);

  // Sort flop
  int flop_ranks[3];
  for (int i = 0; i < 3; ++i)
    flop_ranks[i] = int(cards[i]) / 4;

  std::sort(flop_ranks, flop_ranks + 3);

  // Encode sorted flop
  for (int i = 0; i < 3; ++i)
    community_cards = (community_cards << 4) | flop_ranks[i];

  // Encode turn (if exists)
  if (n >= 4) {
    int turn_rank = int(cards[3]) / 4;
    community_cards = (community_cards << 4) | turn_rank;
  }

  // Encode river (if exists)
  if (n >= 5) {
    int river_rank = int(cards[4]) / 4;
    community_cards = (community_cards << 4) | river_rank;
  }
}

/*
 * Encode a 2-bit representation of a flush status.
 *
 * 00 -> no flush
 * 01 -> bdfd
 * 10 -> fdfd
 * 11 -> flush
 */
void EncodeFlushStatus(uint8_t &flush_status, int suit1, int suit2,
    const std::vector<Card> &community_cards) {
  std::vector<int> suit_counts(4);
  suit_counts[suit1] += 1;
  suit_counts[suit2] += 1;
  for (Card c : community_cards)
    suit_counts[int(c) % 4] += 1;
  int mx = *std::max_element(suit_counts.begin(), suit_counts.end());
  if (mx >= 5)
    flush_status = 3;
  else if (mx >= 4)
    flush_status = 2;
  else if (mx >= 3)
    flush_status = 1;
  else
    flush_status = 0;
}

/*
 * Encode a 2-bit representation of a straight status.
 *
 * 00 -> no straight
 * 01 -> backdoor straight draw (3 in a row)
 * 10 -> frontdoor straight draw (4 in a row)
 * 11 -> made straight (5+ in a row)
 */
void EncodeStraightStatus(uint8_t &straight_status, int rank1, int rank2,
                          const std::vector<Card> &community_cards) {
  // 2...A = 1...13.
  // 0 = A for wheel
  uint16_t rank_mask = 0;

  // Add hole cards
  rank_mask |= (1 << (rank1 + 1));
  if (rank1 == 12)
    rank_mask |= 1;
  rank_mask |= (1 << (rank2 + 1));
  if (rank2 == 12)
    rank_mask |= 1;

  // Add community cards
  for (Card c : community_cards) {
    int rank = int(c) / 4;
    rank_mask |= (1 << (rank + 1));
    if (rank == 12)
      rank_mask |= 1;
  }

  static const uint16_t straight_masks[10] = {
    0b00000000011111, // A2345
    0b00000000111110, // 23456
    0b00000001111100, // 34567
    0b00000011111000, // 45678
    0b00000111110000, // 56789
    0b00001111100000, // 6789T
    0b00011111000000, // 789TJ
    0b00111110000000, // 89TJQ
    0b01111100000000, // 9TJQK
    0b11111000000000  // TJQKA
  };

  int max_overlap = 0;

  for (int i = 0; i < 10; ++i) {
    uint16_t masked = rank_mask & straight_masks[i];
    int overlap = __builtin_popcount(masked);
    max_overlap = std::max(max_overlap, overlap);
  }

  if (max_overlap >= 5)
    straight_status = 3;
  else if (max_overlap == 4)
    straight_status = 2;
  else if (max_overlap == 3)
    straight_status = 1;
  else
    straight_status = 0;
}

/*
 * Encode a 2-bit representation of a flush status. 
 *
 * Simple encoding: return max count of suits - 1
 */
void EncodeBoardFlushStatus(uint8_t &flush_status, const std::vector<Card>
    &community_cards) {
  std::vector<int> suit_counts(4);
  for (Card c : community_cards)
    suit_counts[int(c) % 4] += 1;
  int mx = *std::max_element(suit_counts.begin(), suit_counts.end());
  if (mx >= 5)
    flush_status = 4;
  else
    flush_status = mx - 1;
}

void EncodeHistory(uint64_t &history, const std::vector<int> &action_history) {
  int h = action_history.size() * 3;
  assert(h <= 64);

  for (int action_idx : action_history) {
    assert(action_idx <= (1 << 3));
    history = (history << 3) | action_idx;
  }
}

PublicInfoKey U128Abstraction::GetPublicKey(const GameState& state) const {
  PublicInfoKey key = 0;
  
  uint32_t community_cards;  // 5 * 4 = 20 bits
  EncodeBoardRanks(community_cards, state.community_cards);

  uint8_t flush_status;  // 2 bits
  EncodeBoardFlushStatus(flush_status, state.community_cards);
  assert(flush_status <= 4);

  uint64_t history;
  EncodeHistory(history, state.history);

                                        // num bits used
  key = key         | community_cards;  // 20
  key = (key << 2)  | flush_status;     // 22
  key = (key << 64) | history;          // 86

  return key;
}

PrivateInfoKey U128Abstraction::GetPrivateKey(const GameState &state,
    const std::vector<Card> &hole_cards) const {
  PrivateInfoKey key = 0;
  
  int card1 = int(hole_cards[0]);
  int card2 = int(hole_cards[1]);
  if (card1 < card2)
    std::swap(card1, card2);

  int rank1 = card1 / 4;
  int suit1 = card1 % 4;

  int rank2 = card2 / 4;
  int suit2 = card2 % 4;

  uint8_t cards;  // 2 * 4 = 8 bits
  EncodeHoleCards(cards, rank1, rank2);

  uint8_t flush_status;  // 2 bits
  EncodeFlushStatus(flush_status, suit1, suit2, state.community_cards);
  assert(flush_status <= 3);

  uint8_t straight_status;  // 2 bits
  EncodeStraightStatus(straight_status, rank1, rank2, state.community_cards);
  assert(straight_status <= 3);

                                        // num bits used
  key = key         | cards;            // 8
  key = (key << 2)  | flush_status;     // 30
  key = (key << 2)  | straight_status;  // 32

  return key;
}
