#include "mcts.h"

#include <cassert>

namespace blunder {

// TODO: make this into a proper class to protect invariants when I settle down
// on its API.
struct Node {
  explicit
  Node(const Board& board) noexcept
    : board(board) {}

  Board board;
  std::vector<Node> children;
  MoveProb prob;
  Node* parent = nullptr;
  unsigned visit_cnt = 1;
  unsigned total_val = 0;
  unsigned mean_val = 0;
  bool is_leaf = true;

  Node*
  choose_next() noexcept
  { return is_leaf ? nullptr : choose_action(); }

  // TODO: implement action=argmax(Q(st, a) + U(st,a)).
  Node*
  choose_action() noexcept
  { return nullptr; }

  // TODO: implement expand.
  void
  expand(const Prediction& pred) noexcept
  { (void)pred; }

  // TODO: implement is_terminal.
  bool
  is_terminal() const noexcept
  { return false; }

  // TODO: implement terminate.
  void
  terminate() noexcept
  { return; }
};

class GameTree {
public:
  explicit
  GameTree(const Board& board, const BoardPath& bp)
    : root(board),
      board_path(&bp)
  { assert(bp.size() > 0); }

  // Guaranteed to be non-null.
  Node*
  get_root() noexcept
  { return &root; }

  // TODO: implement get_path.
  BoardPath
  get_path(Node* node) const noexcept
  {
    (void)node;
    return BoardPath();
  }

  // TODO: implement update_stats.
  void
  update_stats(Node* node)
  { (void)node; }

private:
  Node root;
  // Guaranteed to non-null.
  const BoardPath* board_path;
};

SearchResult
Mcts::run(const BoardPath& board_path) const
{
  auto board = board_path.root();
  if (not board)
    throw std::invalid_argument("BoardPath should have a root.");

  GameTree game_tree(board->get(), board_path);

  for (unsigned i = 0; i < simuls; ++i) {
    auto* node = game_tree.get_root();

    while (not node->is_leaf and not node->is_terminal()) {
      node = node->choose_next();
      assert(node);
    }

    // We reached a terminal state so there is no need to call evaluator.
    if (node->is_terminal()) {
      node->terminate();
      game_tree.update_stats(node);
      continue;
    }

    // Reached a leaf node.
    auto bp = game_tree.get_path(node);

    // Evaluate leaf node.
    auto pred = evaluator->predict(bp);

    // Expand leaf node.
    node->expand(pred);

    // Update stats.
    game_tree.update_stats(node);
  }

  // High level algo:
  // - Make root node for board.
  // - for simulation in simulations
  //   - traverse tree
  //   - if reach a leaf node expand and score
  return SearchResult();
}

} // namespace blunder
