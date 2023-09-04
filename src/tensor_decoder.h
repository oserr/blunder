#pragma once

#include <utility>
#include <vector>

#include <torch/torch.h>

#include "board.h"

namespace blunder {

// The decoded moves and value from tensor representations.
struct DecodedMoves {
  std::vector<std::pair<Board, float>> move_probs;
  float value;
};

// An interface for decoding the output Tensors from a neural network with value
// and policy heads back into boards representing the results of the moves, and
// the value of the position.
class TensorDecoder {
public:
  virtual ~TensorDecoder() = default;

  // Converts |board_path| into a Tensor.
  virtual DecodedMoves
  decode(
      const torch::Tensor& mv_tensor,
      const torch::Tensor& eval_tensor) const = 0;
};

} // namespace blunder
