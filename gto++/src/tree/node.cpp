#include "tree/node.h"

bool Node::IsChance() const {
  return flags & 2;
}

bool Node::IsTerminal() const {
  return flags & 1;
}
