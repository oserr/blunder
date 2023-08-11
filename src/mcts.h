#pragma once

#include <memory>
#include <utility>

#include "board_path.h"
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
  run(const BoardPath& board_path) const override;

private:
  std::shared_ptr<Evaluator> evaluator;
  unsigned simuls;
};

} // namespace blunder
