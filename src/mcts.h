#pragma once

#include <memory>
#include <span>
#include <utility>

#include "board.h"
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
  // Adds noise to the priors of the root node before running the simulations.
  void
  add_noise(std::span<std::pair<Board, float>> priors) const;

  std::shared_ptr<Evaluator> evaluator;
  unsigned simuls;
};

} // namespace blunder
