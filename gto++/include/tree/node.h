#pragma once
#include <memory>
#include <vector>
#include <optional>
#include "utils/utils.h"
#include "info_set/info_set_abstraction.h"
#include "game/game_state.h"
class Node;
using NodePtr     = std::shared_ptr<Node>;
using NodeIdx     = std::size_t;
using EdgeIdx     = std::size_t;
using NodeMap     = std::unordered_map<NodeIdx, std::vector<NodeIdx>>;
using TerminalIdx = std::size_t;
using Player      = uint8_t;

struct Node {
  EdgeIdx fst_child;
  uint8_t num_children;
  uint8_t flags;  // 0th bit stores is_terminal, 1th bit stores is_chance
  Player current_player;
  TerminalIdx terminal_utility_matrix_idx; // valid index if terminal, else -1
  bool IsChance() const;
  bool IsTerminal() const;
};
