#include <algorithm>
#include <cassert>
#include <unordered_set>
#include <stack>
#include "tree/tree.h"
#include "game/game_model.h"
#include "utils/utils.h"

Tree::Tree(GameState initial_state,
           int max_raises,
           std::unique_ptr<ActionAbstraction> action_abst,
           std::unique_ptr<InfoSetAbstraction> info_set_abst)
    : initial_state_(std::move(initial_state)),
      max_raises_(max_raises),
      action_abst_(std::move(action_abst)),
      info_set_abst_(std::move(info_set_abst))
{
    // Pre-allocate to avoid repeated reallocation during Build().
    nodes_.reserve(1'000'000);
    edges_.reserve(1'000'000);
}

PublicInfoKey Tree::GetPublicInfoKey(const Cards &community_cards,
    const std::vector<int> &history) const {
  return info_set_abst_->GetPublicInfoKey(community_cards, history);
}
PrivateInfoKey Tree::GetPrivateInfoKey(const Cards &community_cards,
        const Cards &hole_cards) const {
  return info_set_abst_->GetPrivateInfoKey(community_cards, hole_cards);
}

TerminalIdx Tree::AllocTerminalUtilityMatrix(const GameState& state) {
  const Cards& board = state.community_cards;

  auto hands_by_key = info_set_abst_->GetHandsByPrivateInfoKey(board);

  std::vector<PrivateInfoKey> keys;
  keys.reserve(hands_by_key.size());
  for (auto &[k,_] : hands_by_key) keys.push_back(k);
  std::sort(keys.begin(), keys.end());

  int n = keys.size();
  TerminalUtilityMatrix T(n, n);
  T.setZero();

  Chips b0 = state.current_bets[0];
  Chips b1 = state.current_bets[1];
  Chips c0 = state.pot_contributions[0];
  Chips c1 = state.pot_contributions[1];

  // Fold terminals
  if (b0 < b1 || b1 < b0) {
    double util = (b0 < b1) ? -(c0 + b0) : (c1 + b1);
    T.setConstant(util);
    terminal_utility_matrices_.push_back(T);
    return terminal_utility_matrices_.size() - 1;
  }

  // Precompute hand strengths
  std::vector<std::vector<phevaluator::Rank>> strengths(n);

  for (int i = 0; i < n; i++) {
    const auto& hands = hands_by_key.at(keys[i]);
    auto& s = strengths[i];
    s.reserve(hands.size());

    for (const Cards& h : hands) {
      s.push_back(phevaluator::EvaluateCards(
        h[0], h[1],
        board[0], board[1], board[2],
        board[3], board[4]));
    }
  }

  // Pairwise comparison
  for (int i = 0; i < n; i++) {
    const auto& si = strengths[i];

    for (int j = i; j < n; j++) {
      const auto& sj = strengths[j];

      double sum = 0;
      int m = si.size() * sj.size();
      if (i == j) m = si.size() * (si.size() - 1) / 2;

      if (m == 0) continue;

      if (i == j) {
        for (size_t a = 0; a < si.size(); a++)
          for (size_t b = a + 1; b < si.size(); b++)
            if (si[a] < si[b]) sum += c1;
            else if (si[b] < si[a]) sum -= c0;
      }
      else {
        for (auto r0 : si)
          for (auto r1 : sj)
            if (r0 < r1) sum += c1;
            else if (r1 < r0) sum -= c0;
      }

      T(i, j) = sum / m;
      if (i != j) T(j, i) = -T(i, j);
    }
  }

  terminal_utility_matrices_.push_back(T);
  return terminal_utility_matrices_.size() - 1;
}

/*
 * Appends a new Node to nodes_[] for `state`.
 * fst_child / num_children are left as sentinel values; BuildIterative()
 * fills them in once the children are known.
 */
NodeIdx Tree::AllocNode(const GameState& state) {
  NodeIdx idx = nodes_.size();

  uint8_t flags = (state.IsChance()   ? Node::kChanceBit   : 0)
                | (state.is_terminal  ? Node::kTerminalBit : 0);

  nodes_.push_back(Node{
      /* fst_child                    = */ kInvalidEdge,
      /* num_children                 = */ 0,
      /* flags                        = */ flags,
      /* current_player               = */ static_cast<Player>(state.current_player),
      /* terminal_utility_matrix_idx  = */ kInvalidTerminal,
      });

  return idx;
}

namespace {

// Sentinel meaning "no parent" (used for the root node).
static constexpr NodeIdx kNoParent = SIZE_MAX;

struct Frame {
    GameState state;
    NodeIdx   parent_idx;   // index of the parent node in nodes_[]
    EdgeIdx   edge_slot;    // index in edges_[] where this node's idx goes
                            // (SIZE_MAX for root)
};

} // namespace

/*
 * Single-pass iterative DFS that writes nodes and edges in pre-order.
 *
 * When we first visit a node we record edges_begin = edges_.size().
 * We then push all children onto the DFS stack.  As each child is
 * processed it appends its own NodeIdx to edges_ (via a small
 * "pending parent" mechanism described below).  When we finish
 * expanding a node we can therefore set
 *     fst_child  = edges_begin
 *     num_children = edges_.size() - edges_begin
 * because the child NodeIdxs were appended in order.
 *
 * We need "deferred" writes because a child's NodeIdx isn't known until
 * we actually allocate it.  We handle this with a two-field stack frame:
 *
 *   struct Frame {
 *     GameState state;       // state for the node we are about to create
 *     NodeIdx   parent;      // parent node (kNoParent for root)
 *   };
 *
 * When we pop a frame we:
 *   1. Check visited set (key deduplication via canonical suit abstraction).
 *   2. AllocNode -> child_idx.
 *   3. Append child_idx to edges_  (this is the "edge from parent to child").
 *   4. Record edges_begin for this node.
 *   5. Push all successor frames.
 *   6. After pushing successors, set fst_child / num_children on this node
 *      using the fact that the children's NodeIdxs will occupy
 *      edges_[edges_begin .. edges_begin + num_successors).
 *
 * Because we push children in reverse order and DFS is LIFO, children are
 * visited (and their NodeIdxs appended to edges_) in the correct
 * left-to-right order.
 */
void Tree::BuildIterative() {
  int x = 0;
  std::unordered_set<PublicInfoKey> have;

  // visited: canonical PublicInfoKey -> NodeIdx
  // We need NodeIdx (not just bool) so that when the same canonical state
  // is reached via a different path we can reuse the existing subtree.
  // For a standard NLH subgame tree this doesn't happen (the tree is a
  // DAG only at chance nodes with suit-isomorphic runouts), but the
  // canonical abstraction CAN map two distinct raw game states to the
  // same key. In that case we skip the duplicate.
  std::unordered_set<PublicInfoKey> visited;
  visited.reserve(1'000'000);

  std::stack<Frame> dfs;

  // Seed with the root.
  dfs.push(Frame{
      initial_state_,
      kNoParent,
      static_cast<EdgeIdx>(SIZE_MAX)  // root has no parent edge slot
      });

  while (!dfs.empty()) {
    Frame frame = std::move(dfs.top());
    dfs.pop();

    const GameState &state = frame.state;

    // Compute canonical key for deduplication.
    PublicInfoKey key = info_set_abst_->GetPublicInfoKey(
        state.community_cards,
        state.history
        );

    // If this canonical state has already been built, write the existing
    // node index into the parent's edge slot and skip expansion.
    // (This handles suit-isomorphic subtree sharing.)
    if (visited.count(key)) {
      // The existing node's idx was already emitted when it was first
      // visited; we don't need to emit it again here.  However, for a
      // pure tree (no DAG sharing) this branch is never taken.
      // If you want DAG sharing, store key->NodeIdx and write it here.
      continue;
    }
    visited.insert(key);

    // Allocate the node.
    NodeIdx curr_idx = AllocNode(state);

    // Write this node's index into the parent's edge slot.
    if (frame.edge_slot != static_cast<EdgeIdx>(SIZE_MAX)) {
      edges_[frame.edge_slot] = curr_idx;
    } else {
      // This is the root.
      root_idx_ = curr_idx;
    }

    if (state.is_terminal) {
      // No children; leave fst_child = kInvalidEdge, num_children = 0.
      BoardKey board_key = info_set_abst_->GetBoardKey(state.community_cards);

      if (terminal_utility_matrix_indices_.contains(board_key))
        nodes_[curr_idx].terminal_utility_matrix_idx = terminal_utility_matrix_indices_.at(key);
      else {
        // Allocate a terminal util matrix
        std::cout << "Allocating terminal matrix\n";
        TerminalIdx idx = AllocTerminalUtilityMatrix(state);
        std::cout << "Done terminal matrix\n";
        nodes_[curr_idx].terminal_utility_matrix_idx = idx;
        terminal_utility_matrix_indices_[key] = idx;
      }

      continue;
    }

    // Collect successor states.
    // We reserve edge slots in edges_[] for the children *before* we
    // know their NodeIdxs, then fill them in as children are processed.
    std::vector<GameState> successors;

    if (state.IsChance()) {
      // Chance node: deal each possible next card.

      // keep track of seen keys, to avoid overcounting isomorphic next states
      std::unordered_set<PublicInfoKey> seen;
      for (Card card : CARDS) {
        if (!Contains(state.community_cards, card)) {
          GameState next = GameModel::Step(state, card);
          PublicInfoKey next_key = info_set_abst_->GetPublicInfoKey(
              next.community_cards, next.history);
          if (!visited.count(next_key) && !seen.count(next_key)) {
            seen.insert(next_key);
            successors.push_back(std::move(next));
          }
        }
      }
    } else {
      // Player node: enumerate legal actions.
      auto actions = action_abst_->GetActions(state);
      int action_idx = 0;
      for (const auto &action : actions) {
        if (GameModel::IsLegal(state, action, max_raises_)) {
          GameState next = GameModel::Step(state, action, action_idx);
          PublicInfoKey next_key = info_set_abst_->GetPublicInfoKey(
              next.community_cards, next.history);
          if (!visited.count(next_key)) {
            successors.push_back(std::move(next));
          }
          ++action_idx;
        }
      }
    }

    const int n = static_cast<int>(successors.size());
    assert(n <= 255 && "num_children overflows uint8_t");

    // Reserve n contiguous slots in edges_[] for the children.
    // The slots are currently SIZE_MAX (unset); BuildIterative will
    // overwrite them as it processes each child frame.
    EdgeIdx fst = edges_.size();
    edges_.resize(edges_.size() + n, SIZE_MAX);

    nodes_[curr_idx].fst_child    = fst;
    nodes_[curr_idx].num_children = static_cast<uint8_t>(n);

    // Push children in REVERSE order so that the leftmost child is
    // popped (and processed) first, preserving action order.
    for (int i = n - 1; i >= 0; --i) {
      dfs.push(Frame{
          std::move(successors[i]),
          curr_idx,
          static_cast<EdgeIdx>(fst + i)  // the slot this child must fill
          });
    }
  }
}

void Tree::Build() {
  nodes_.clear();
  edges_.clear();
  BuildIterative();
}
