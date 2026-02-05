#pragma once

#include <vector>
#include "node.h"

class ActionAbstraction {
public:
  std::vector<Action> GetActions(const Node& u);
};
