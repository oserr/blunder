#pragma once

#include <random>

#include "search.h"
#include "board_path.h"

namespace blunder {

// Chooses moves randomly. Serves two purposes:
// 1. Allows me to develop other interface components more quickly because it
//    will allow me to see how this hooks up with other components, and I can
//    even work on setting up some sort of UI to play gainst the engine.
// 2. Eventually, when I start training the engine for real, I can also
//    measure the peformance of the trained engine against the random engine.
class RandomSearch : public Search {
public:
  // Initialzies RandomSearch with a seed for the random number generator.
  explicit
  RandomSearch(unsigned seed)
    : rand_fn(seed),
      rand_value_fn(-1.0, 1.0) {}

  SearchResult
  run(const EvalBoardPath& board_path) const override;

private:
  mutable std::mt19937 rand_fn;
  mutable std::uniform_real_distribution<float> rand_value_fn;
};

} // namespace blunder
