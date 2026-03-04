#pragma once
#include <memory>

#include <cstddef>
#include <cstdint>

#include "utils/utils.h"
#include "game/game_state.h"

class Node;
using NodePtr       = std::shared_ptr<Node>;
using NodeIdx       = std::size_t;
using EdgeIdx       = std::size_t;
using Player        = uint8_t;
using TerminalIdx   = std::size_t;

// Represents an empty edge
static constexpr EdgeIdx     kInvalidEdge     = SIZE_MAX;
// Represents a non-terminal state
static constexpr TerminalIdx kInvalidTerminal = SIZE_MAX;

struct Node {
    // Flag bit positions.
    static constexpr uint8_t kTerminalBit = 1u << 0;
    static constexpr uint8_t kChanceBit   = 1u << 1;

    EdgeIdx     fst_child;                   // index into edges_[] of first child
    uint8_t     num_children;
    uint8_t     flags;                       // bit 0: terminal, bit 1: chance
    Player      current_player;
    TerminalIdx terminal_utility_matrix_idx; // kInvalidTerminal if not terminal

    bool IsTerminal() const { return flags & kTerminalBit; }
    bool IsChance()   const { return flags & kChanceBit;   }
};

