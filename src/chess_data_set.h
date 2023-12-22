#pragma once

#include <cstddef>
#include <memory>
#include <span>
#include <stdexcept>
#include <utility>
#include <vector>

#include "game_result.h"
#include "tensor_encoder.h"

#include <torch/torch.h>

namespace blunder {

using TensorPair = std::pair<torch::Tensor, torch::Tensor>;
using ChessDataExample = torch::data::Example<torch::Tensor, TensorPair>;

// TODO: When we begin to generate training data through self play, we'll save
// the game history, and this custom loader will need to read the data and
// convert it to tensors.
//
// Placer holder code for a custom dataset to use with a dataloader.
// Each example will consists of the input to the network, i.e. an 8x8x119 stack
// of planes representing the current chess position, and the target will
// consist of a pair of tensors in the form of (policy target, value target) to
// represent the expected policy and value targets.
class ChessDataSet :
  public torch::data::datasets::Dataset<ChessDataSet, ChessDataExample>
{
public:
  ChessDataSet(
      std::span<const GameResult> game_results,
      std::shared_ptr<TensorEncoder> encoder);

  //static constexpr bool is_stateful = false;

  ExampleType
  get(std::size_t index) override;

  torch::optional<std::size_t>
  size() const override
  { return num_examples; };

private:
  // Note that this is not owned and hence the game results are expected to
  // outlive ChessDataSet.
  std::span<const GameResult> game_results;
  std::shared_ptr<TensorEncoder> encoder;
  std::size_t num_examples = 0;
};

// Converts a collection of ChessDataExamples into a single Example by stacking
// all the tensors as a single tensor.
ChessDataExample
stack_examples(std::vector<ChessDataExample> examples);

} // namespace blunder
