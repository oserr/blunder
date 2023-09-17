#include "alpha_zero_evaluator.h"

#include <stdexcept>
#include <utility>

namespace blunder {

Prediction
AlphaZeroEvaluator::predict(const BoardPath& board_path) const
{
  auto root = board_path.root();
  if (not root)
    throw std::invalid_argument("board_path should have at least one board.");

  auto input_tensor = tensor_encoder->encode_state(board_path);
  auto [policy_tensor, value_tensor] = net->forward(input_tensor);

  auto decoded_moves = tensor_decoder->decode(
          *root, std::move(policy_tensor), std::move(value_tensor));

  return Prediction{
    .move_probs=std::move(decoded_moves.move_probs),
    .value=decoded_moves.value
  };
}

} // namespace blunder
