#include "tree/packed_tree.h"
#include "tree/tree.h"

PackedTree BuildPackedTree(const Tree &tree) {
  PackedTree pt;
  const std::size_t n = tree.Size();

  pt.is_terminal.resize(n);
  pt.is_chance.resize(n);
  pt.current_player.resize(n);
  pt.fst_child.resize(n);
  pt.num_children.resize(n);
  pt.terminal_idx.resize(n);

  // Copy edge list directly via public accessors.
  // edges are implicit via Child(), so we rebuild them in node order.
  pt.root = tree.Root();

  std::vector<NodeIdx> queue;
  queue.reserve(n);
  queue.push_back(pt.root);

  std::vector<uint8_t> visited(n, 0);
  visited[pt.root] = 1;

  while (!queue.empty()) {
    NodeIdx u = queue.back();
    queue.pop_back();

    pt.is_terminal[u]    = tree.IsTerminal(u) ? 1 : 0;
    pt.is_chance[u]      = tree.IsChance(u) ? 1 : 0;
    pt.current_player[u] = static_cast<Player>(tree.CurrentPlayer(u));
    pt.num_children[u]   = static_cast<uint8_t>(tree.NumChildren(u));
    // fst_child is index into pt.edges; append children now.
    pt.fst_child[u]     = static_cast<EdgeIdx>(pt.edges.size());
    pt.terminal_idx[u]  = tree.TerminalMatrixIndex(u);

    const int k = tree.NumChildren(u);
    for (int i = 0; i < k; ++i) {
      NodeIdx v = tree.Child(u, i);
      pt.edges.push_back(v);
      if (!visited[v]) {
        visited[v] = 1;
        queue.push_back(v);
      }
    }
  }

  return pt;
}

