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

  // Converts |board| into a Tensor.
  virtual torch::Tensor
  encode_board(const Board& board) const = 0;

  // Converts |board_path| into a Tensor.
  virtual torch::Tensor
  encode_state(const EvalBoardPath& board_path) const = 0;

  // Converts |moves| into a Tensor.
  virtual torch::Tensor
  encode_moves(std::span<const BoardProb> moves) const = 0;

  // Converts |moves| into a Tensor.
  virtual torch::Tensor
  encode_moves(std::span<const MoveProb> moves) const = 0;

  // If set, tensors are encoded with requires_grad=true, otherwise they are
  // encoded with requires_grad=false.
  void with_grad(bool enabled)
  { with_grad_enabled = enabled; }

protected:
  bool with_grad_enabled = false;
};

} // namespace blunder
