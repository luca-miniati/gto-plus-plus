#pragma once
#include "tree/node.h"
#include "info_set/info_set.h"

class InfoSetAbstraction {
  public:
    virtual ~InfoSetAbstraction() = default;

    virtual NodeKey GetPublicKey(const GameState &state) const = 0;

    virtual NodeKey GetPrivateKey(const GameState &state,
        const std::vector<Card> &hole_cards) const = 0;
};

class U128Abstraction : public InfoSetAbstraction {
  public:
    NodeKey GetPublicKey(const GameState &state) const override;

    NodeKey GetPrivateKey(const GameState &state,
        const std::vector<Card> &hole_cards) const override;
};
