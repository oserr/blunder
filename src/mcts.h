#pragma once

#include <memory>
#include <utility>

#include "evaluator.h"
#include "search.h"

namespace blunder {

// Monte Carlo Tree Search.
class Mcts : public Search {
public:
  Mcts(std::shared_ptr<Evaluator> evaluator, unsigned simulations) noexcept
    : evaluator(std::move(evaluator)),
      simuls(simulations) {}

  SearchResult
  run(const Board& board) const override;

private:
  std::shared_ptr<Evaluator> evaluator;
  unsigned simuls;
};

} // namespace blunder
