#pragma once

#include <memory>
#include <utility>

#include "evaluator.h"
#include "search.h"

namespace blunder {

struct Node {
  Board board;
  std::vector<Node> children;
  MoveProb prob;
  Node* parent = nullptr;
  unsigned visit_cnt = 1;
  unsigned total_val = 0;
  unsigned mean_val = 0;
  bool is_leaf = false;
};

// Monte Carlo Tree Search.
class Mcts : public Search {
public:
  Mcts(std::shared_ptr<Evaluator> evaluator, unsigned simulations)
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
