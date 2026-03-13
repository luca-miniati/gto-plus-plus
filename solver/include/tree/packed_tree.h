#pragma once
#include <vector>
#include <cstdint>
#include "tree/node.h"

// A solver-oriented packed view of a Tree.
struct PackedTree {
  // Node-level arrays (structure-of-arrays style).
  std::vector<uint8_t>  is_terminal;
  std::vector<uint8_t>  is_chance;
  std::vector<Player>   current_player;
  std::vector<EdgeIdx>  fst_child;
  std::vector<uint8_t>  num_children;
  std::vector<TerminalIdx> terminal_idx;

  // Edge list: child node indices.
  std::vector<NodeIdx>  edges;

  NodeIdx root;
};

PackedTree BuildPackedTree(const class Tree &tree);

