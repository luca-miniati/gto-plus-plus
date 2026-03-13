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

void Tree::AllocShowdownResultMatrix(
    const GameState &state,
    const std::map<PrivateInfoKey, std::vector<Cards>> &hands_by_key) {

  std::vector<PrivateInfoKey> keys;
  keys.reserve(hands_by_key.size());
  for (auto &[k, _] : hands_by_key)
    keys.push_back(k);
  std::sort(keys.begin(), keys.end());

  int n = keys.size();
  Eigen::MatrixXd S(n, n);
  S.setZero();

  // Step 1: precompute hand strengths for each info set
  std::vector<std::vector<phevaluator::Rank>> hand_strengths(n); // hand_strengths[i][h] = strength
  for (int i = 0; i < n; ++i) {
    const auto &hands = hands_by_key.at(keys[i]);
    hand_strengths[i].reserve(hands.size());
    for (const Cards &h : hands) {
      hand_strengths[i].push_back(
          phevaluator::EvaluateCards(
              h[0],
              h[1],
              state.community_cards[0],
              state.community_cards[1],
              state.community_cards[2],
              state.community_cards[3],
              state.community_cards[4]));
    }
  }

  // Step 2: compute matrix
  // i < j
  for (int i = 0; i < n; ++i) {
    for (int j = i + 1; j < n; ++j) {
      double sum = 0.0;
      const auto &hi = hand_strengths[i];
      const auto &hj = hand_strengths[j];

      for (int ii = 0; ii < hi.size(); ++ii) {
        for (int jj = 0; jj < hj.size(); ++jj) {
          phevaluator::Rank a = hi[ii];
          phevaluator::Rank b = hj[jj];
          if (a > b)
            sum += 1;
          else if (a < b)
            sum -= 1;
          // tie: sum += 0
        }
      }

      double val = sum / (hi.size() * hj.size());
      S(i, j) = val;
      S(j, i) = -val;
    }
  }

  // i = j
  for (int i = 0; i < n; ++i) {
    double sum = 0.0;
    const auto &h = hand_strengths[i];
    if (h.size() <= 2)
      continue; // if there are 2 or fewer hands in this info set, there are no pairs of distinct hands

    for (int ii = 0; ii < h.size(); ++ii) {
      for (int jj = ii + 1; jj < h.size(); ++jj) {
        phevaluator::Rank a = h[ii];
        phevaluator::Rank b = h[jj];
        if (a > b)
          sum += 1;
        else if (a < b)
          sum -= 1;
      }
    }

    // divide by |h| choose 2, which is the number of pairs of hands from the same info set
    S(i, i) = sum / (h.size() * (h.size() - 1) / 2);
  }

  // store in map
  ShowdownKey key = info_set_abst_->GetShowdownKey(state.community_cards);
  showdown_result_matrices_[key] = S;
}

TerminalIdx Tree::AllocTerminalUtilityMatrix(
    const GameState &state,
    const ShowdownKey &showdown_key,
    const std::map<PrivateInfoKey, std::vector<Cards>> &hands_by_key) {
  int n = hands_by_key.size();
  Eigen::MatrixXd T(n, n);
  Eigen::MatrixXd S = showdown_result_matrices_.at(showdown_key);

  Chips half_of_pot_size = state.pot_contributions[0];

  for (int i = 0; i < n; ++i)
    for (int j = 0; j < n; ++j)
      T(i, j) = S(i, j) * half_of_pot_size;
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
    PublicInfoKey state_key = info_set_abst_->GetPublicInfoKey(
        state.community_cards,
        state.history
        );

    // If this canonical state has already been built, write the existing
    // node index into the parent's edge slot and skip expansion.
    // (This handles suit-isomorphic subtree sharing.)
    if (visited.count(state_key)) {
      // The existing node's idx was already emitted when it was first
      // visited; we don't need to emit it again here.  However, for a
      // pure tree (no DAG sharing) this branch is never taken.
      // If you want DAG sharing, store key->NodeIdx and write it here.
      continue;
    }
    visited.insert(state_key);

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
      if (terminal_utility_matrix_indices_.count(state_key))
        nodes_[curr_idx].terminal_utility_matrix_idx = terminal_utility_matrix_indices_.at(state_key);
      else {
        auto hands_by_key = info_set_abst_->GetHandsByPrivateInfoKey(state.community_cards);

        if (state.current_bets[0] != state.current_bets[1]) {
          // If it's a fold terminal, we don't need to compute showdown results.
          // Just allocate a terminal util matrix with the correct constant value.
          Chips b0 = state.current_bets[0];
          Chips b1 = state.current_bets[1];
          Chips c0 = state.pot_contributions[0];
          Chips c1 = state.pot_contributions[1];
          double util = (b0 < b1) ? -(c0 + b0) : (c1 + b1);

          int n = hands_by_key.size();
          Eigen::MatrixXd T(n, n);
          T.setConstant(util);

          TerminalIdx idx = terminal_utility_matrices_.size();
          terminal_utility_matrices_.push_back(T);
          nodes_[curr_idx].terminal_utility_matrix_idx = idx;
          terminal_utility_matrix_indices_[state_key] = idx;
        } else {
          ShowdownKey showdown_key = info_set_abst_->GetShowdownKey(state.community_cards);

          if (!showdown_result_matrices_.count(showdown_key))
            AllocShowdownResultMatrix(state, hands_by_key);

          // Allocate a terminal util matrix
          TerminalIdx idx = AllocTerminalUtilityMatrix(state, showdown_key, hands_by_key);
          nodes_[curr_idx].terminal_utility_matrix_idx = idx;
          terminal_utility_matrix_indices_[state_key] = idx;
        }
      }

      // No children; leave fst_child = kInvalidEdge, num_children = 0.
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
