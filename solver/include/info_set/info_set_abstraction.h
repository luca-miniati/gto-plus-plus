#pragma once
#include "info_set/info_set.h"
#include "utils/uint128.h"
using PublicInfoKey   = UInt128;
using PrivateInfoKey  = uint16_t;

/*
 * Both types of Info Keys (Public Info Keys and Private Info Keys) are used to
 * represent groups of information sets that are "strategically equivalent".
 *
 * For example, consider the game state "Ace-king suited on A46 rainbow with a
 * backdoor flush draw". There's 12 different ways this information set can
 * happen, but the strategy should be the exact same for all of them. So, our
 * strategy for those game states should be stored under the same key.
 *
 * Public Info Key (128-bit)
 * Represents a GameState, with the hole cards of both players unknown and
 * perfect recall of history.
 * Layout:
 * +----------------------------------------------------------------+
 * |ssssssssssrrrrrrrrrrrrrrrrrrrr aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa|
 * +----------------------------------------------------------------+
 * s = suit (3-bit), 1-indexed
 * r = rank (4-bit), 1-indexed
 * a = action idx (3-bit), 1-indexed
 *
 * Private Info Key (16-bit)
 * Represents information private to the player to act. Since Private Info Keys
 * in different GameStates won't be compared, no public game state information
 * is stored in the Private Info Key. Only hole card information is stored.
 * Layout:
 * +------------+
 * |ssssrrrrrrrr|
 * +------------+
 * s = suit (3-bit), 1-indexed
 * r = rank (4-bit), 1-indexed
 */

class InfoSetAbstraction {
  public:
    virtual ~InfoSetAbstraction() = default;

    virtual PublicInfoKey GetPublicInfoKey(const Cards &community_cards,
        const std::vector<int> &history) const = 0;

    virtual PrivateInfoKey GetPrivateInfoKey(const Cards &community_cards,
        const std::vector<Card> &hole_cards) const = 0;

    virtual std::vector<PrivateInfoKey> GetAllPrivateInfoKeys(
        const Cards &community_cards) const = 0;
};