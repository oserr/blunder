#pragma once

#include <span>

#include <torch/torch.h>

#include "board.h"
#include "board_path.h"
#include "search_result.h"

namespace blunder {

// An interface for encoding the input boards into the tensor input for neural
// network evaluation.
class TensorEncoder {
public:
  virtual ~TensorEncoder() = default;

  // Converts |board_path| into a Tensor.
  virtual torch::Tensor
  encode_state(const EvalBoardPath& board_path) const = 0;

  // Converts |board_path| into a Tensor.
  virtual torch::Tensor
  encode_moves(std::span<const BoardProb> moves) const = 0;
};

} // namespace blunder
