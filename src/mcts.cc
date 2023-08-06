#include "mcts.h"

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
  bool is_leaf = false;
};

class GameTree {
public:
  explicit
  GameTree(const Board& board) noexcept
    : root(board) {}

private:
  Node root;
};

SearchResult
Mcts::run(const BoardPath& board) const
{
  (void)board;
  // High level algo:
  // - Make root node for board.
  // - for simulation in simulations
  //   - traverse tree
  //   - if reach a leaf node expand and score
  return SearchResult();
}

} // namespace blunder
