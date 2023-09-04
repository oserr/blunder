#pragma once

#include <torch/torch.h>

#include "board_path.h"

namespace blunder {

// An interface for encoding the input boards into the tensor input for neural
// network evaluation.
class TensorEncoder {
public:
  virtual ~TensorEncoder() = default;

  // Converts |board_path| into a Tensor.
  virtual torch::Tensor
  encode(const BoardPath& board_path) const = 0;
};

} // namespace blunder
