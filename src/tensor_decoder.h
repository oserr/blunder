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

  // Decodes |mv_tensor|, a tensor of dimension (8, 8, 73) encoding the
  // probabilities for all legal moves from |board|, and decodes |eval_tensor|,
  // which represents the value of the position of in |board|.
  virtual DecodedMoves
  decode(
      const Board& board,
      const torch::Tensor& mv_tensor,
      const torch::Tensor& eval_tensor) const = 0;
};

} // namespace blunder
