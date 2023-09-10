#pragma once

#include <span>

#include "board.h"
#include "board_path.h"
#include "search_result.h"
#include "tensor_encoder.h"

namespace blunder {

class AlphaZeroEncoder : public TensorEncoder {
public:
  // Encodes the input Board as a tensor for evaluation.
  torch::Tensor
  encode_state(const BoardPath& board_path) const override;

  // Encodes the moves for the Board as a tensor for training.
  torch::Tensor
  encode_moves(std::span<const MoveProb> moves) const override;
};

} // namespace
