#pragma once

#include <utility>
#include <vector>

#include "board.h"

namespace blunder {

// Represents the evaluation result of a given chess position.
struct Prediction {
  // Vector of move probabilities.
  std::vector<std::pair<Move, float>> move_probs;

  // A value between [-1, 1] to represent likelihood of winning, drawing, or
  // losing.
  float value;
};

// An abstract interface for evaluating chess positions.
class Evaluator {
public:
  virtual
  ~Evaluator() = default;

  virtual Prediction
  predict(const Board& board) const = 0;
};

} // namespace blunder
