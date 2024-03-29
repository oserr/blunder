#include "chess_data_set.h"

#include <cstddef>
#include <memory>
#include <span>
#include <stdexcept>
#include <utility>

#include "game_result.h"
#include "tensor_encoder.h"

#include <torch/torch.h>

namespace blunder {

using torch::Tensor;

ChessDataSet::ChessDataSet(
    std::span<const GameResult> game_results,
    std::shared_ptr<TensorEncoder> encoder)
  : game_results(game_results),
    encoder(std::move(encoder))
{
  if (not this->encoder)
    throw std::invalid_argument("encoder cannot be null.");

  // Enable encoding tensors with grad enabled.
  this->encoder->with_grad(true);

  for (const auto& gr : game_results)
    num_examples += gr.moves.size();
}

ChessDataSet::ExampleType
ChessDataSet::get(std::size_t index)
{
  if (index >= num_examples)
    throw std::out_of_range("index is out of range");

  const GameResult* game_result = nullptr;

  for (const auto& gr : game_results) {
    const auto num_moves = gr.moves.size();
    if (num_moves > index) {
      game_result = &gr;
      break;
    }
    index -= num_moves;
  }

  if (not game_result)
    throw std::runtime_error("Unable to find the game_result for index.");

  const auto& board = index == 0
      ? game_result->game_start
      : game_result->moves[index-1].best.board;

  auto input_tensor = encoder->encode_board(board).to(torch::kCUDA);
  auto policy_tensor = encoder->encode_moves(game_result->moves[index].moves)
                                .to(torch::kCUDA);

  // Compute the actual value from the game result.
  float value = 0;
  if (game_result->winner == Color::White)
    value = board.is_white_next() ? 1 : -1;
  else if (game_result->winner == Color::Black)
    value = board.is_white_next() ? -1 : 1;

  Tensor value_tensor = torch::full({1}, value, torch::kCUDA);

  return ExampleType(std::move(input_tensor),
                     std::make_pair(policy_tensor, value_tensor));
}

ChessDataExample
stack_examples(std::vector<ChessDataExample> examples)
{
  std::vector<Tensor> data;
  std::vector<Tensor> policies;
  std::vector<Tensor> values;
  data.reserve(examples.size());
  policies.reserve(examples.size());
  values.reserve(examples.size());

  for (auto example : examples) {
    data.push_back(std::move(example.data));
    auto& [policy, value] = example.target;
    policies.push_back(std::move(policy));
    values.push_back(std::move(value));
  }

  return ChessDataExample(
      torch::stack(data),
      std::make_pair(torch::stack(policies), torch::stack(values)));
}

} // namespace blunder
