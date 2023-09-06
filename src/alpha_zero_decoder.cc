#include "alpha_zero_decoder.h"

#include <torch/torch.h>

namespace blunder {

DecodedMoves
AlphaZeroDecoder::decode(
    const torch::Tensor& mv_tensor,
    const torch::Tensor& eval_tensor) const
{
  // TODO: add logic to decode the mv tensor into a vector of boards, each
  // representing the next state after a specific move is applied.
  (void)mv_tensor;
  (void)eval_tensor;
  return DecodedMoves();
}

} // namespace blunder
