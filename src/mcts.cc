#include "mcts.h"

#include <cassert>

namespace blunder {

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
};

class GameTree {
public:
  explicit
  GameTree(const Board& board, const BoardPath& bp)
    : root(board),
      board_path(&bp)
  { assert(bp.size() > 0); }

private:
  Node root;
  // Guaranteed to non-null.
  const BoardPath* board_path;
};

SearchResult
Mcts::run(const BoardPath& board_path) const
{
  auto root = board_path.root();
  if (not root)
    throw std::invalid_argument("BoardPath should have a root.");

  GameTree game_tree(root->get(), board_path);

  // High level algo:
  // - Make root node for board.
  // - for simulation in simulations
  //   - traverse tree
  //   - if reach a leaf node expand and score
  return SearchResult();
}

} // namespace blunder
