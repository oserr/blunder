#pragma once

#include <torch/torch.h>

#include "board.h"
#include "tensor_decoder.h"

namespace blunder {

class AlphaZeroDecoder : public TensorDecoder {
public:
  // Converts |board_path| into a Tensor.
  DecodedMoves
  decode(
      const torch::Tensor& mv_tensor,
      const torch::Tensor& eval_tensor) const override;
};

} // namespace blunder
