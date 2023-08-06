#pragma once

#include <memory>
#include <utility>

#include "evaluator.h"
#include "search.h"

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

// Monte Carlo Tree Search.
class Mcts : public Search {
public:
  Mcts(std::shared_ptr<Evaluator> evaluator, unsigned simulations) noexcept
    : evaluator(std::move(evaluator)),
      simuls(simulations) {}

  SearchResult
  run(const Board& board) const override
  { return SearchResult(); }

private:
  std::shared_ptr<Evaluator> evaluator;
  unsigned simuls;
};

} // namespace blunder
