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
  unsigned visit_cnt;
  unsigned total_val;
  unsigned mean_val;
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
