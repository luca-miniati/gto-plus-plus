#pragma once
#include "tree/node.h"
#include "info_set/info_set.h"

class InfoSetAbstraction {
  public:
    virtual ~InfoSetAbstraction() = default;

    virtual NodeKey GetKey(const InfoSet& i) const = 0;
};

class U128Abstraction : public InfoSetAbstraction {
  public:
    NodeKey GetKey(const InfoSet& i) const override;
};
