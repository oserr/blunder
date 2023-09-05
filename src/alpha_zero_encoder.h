#pragma once

#include "board_path.h"
#include "tensor_encoder.h"

namespace blunder {

class AlphaZeroEncoder : public TensorEncoder {
public:
  // Encodes the input Board as a tensor for evaluation.
  torch::Tensor
  encode(const BoardPath& board_path) const override;
};

} // namespace
