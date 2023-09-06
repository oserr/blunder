#pragma once

#include <torch/torch.h>

#include "board.h"
#include "tensor_decoder.h"

namespace blunder {

// An interface for decoding the output Tensors from a neural network with value
// and policy heads back into boards representing the results of the moves, and
// the value of the position.
class AlphaZeroDecoder : public TensorDecoder {
public:
  // Converts |board_path| into a Tensor.
  DecodedMoves
  decode(
      const torch::Tensor& mv_tensor,
      const torch::Tensor& eval_tensor) const override;
};

} // namespace blunder
