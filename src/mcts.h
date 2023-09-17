#pragma once

#include <functional>
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
  // Initializes the Monte Carlo Tree Search with an evaluator, the number of
  // simulations to run, and a seed for the Dirilecth noise random generator.
  Mcts(
      std::shared_ptr<Evaluator> evaluator,
      unsigned simulations,
      unsigned seed);

  SearchResult
  run(const EvalBoardPath& board_path) const override;

private:
  // Adds noise to the priors of the root node before running the simulations.
  void
  add_noise(std::span<std::pair<Board, float>> priors) const;

  std::shared_ptr<Evaluator> evaluator;
  unsigned simuls;
  std::function<float()> dir_fn;
};

} // namespace blunder
