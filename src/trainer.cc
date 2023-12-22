#include "trainer.h"

#include <filesystem>
#include <format>
#include <iterator>
#include <memory>
#include <span>
#include <string>
#include <stdexcept>
#include <utility>
#include <vector>

#include "alpha_zero_encoder.h"
#include "chess_data_set.h"
#include "game_result.h"
#include "net.h"
#include "simple_game_builder.h"

#include <torch/torch.h>

namespace blunder {

namespace fs = std::filesystem;

using torch::data::transforms::Collate;

// Plays the training games.
std::vector<GameResult>
Trainer::play_training_games(std::shared_ptr<AlphaZeroNet> net) const
{
  c10::InferenceMode inference_mode(true);

  auto game = SimpleGameBuilder()
                .set_net(std::move(net))
                .set_max_moves(max_moves_per_game)
                .set_decoder(decoder)
                .set_encoder(encoder)
                .build();

  std::vector<GameResult> game_results;
  game_results.reserve(training_games);

  for (unsigned i = 0; i < training_games; ++i) {
    auto game_result = game.play();
    game_results.push_back(std::move(game_result));
  }

  return game_results;
}

// Plays the tournament games.
MatchStats
Trainer::play_tournament_games(std::shared_ptr<AlphaZeroNet> contender) const
{
  c10::InferenceMode inference_mode(true);

  auto game = SimpleGameBuilder()
                .set_white_net(champion)
                .set_black_net(std::move(contender))
                .set_max_moves(max_moves_per_game)
                .set_decoder(decoder)
                .set_encoder(encoder)
                .build();

  MatchStats stats;
  auto* white = &stats.champion_wins;
  auto* black = &stats.contender_wins;

  for (unsigned i = 0; i < tournament_games; ++i) {
    auto game_result = game.play();
    if (game_result.winner == Color::White)
      *white += 1;
    else if (game_result.winner == Color::Black)
      *black += 1;
    else
      stats.draws += 1;

    game.flip_colors();
    std::swap(white, black);
  }

  return stats;
}

// Trains the model.
std::shared_ptr<AlphaZeroNet>
Trainer::train_model(
    std::span<const GameResult> game_results,
    const AlphaZeroNet& net) const
{
  // Disable inference mode.
  c10::InferenceMode inference_mode(false);

  auto trained_net = std::make_shared<AlphaZeroNet>(net.clone());
  trained_net->set_training_mode();

  ChessDataSet data_set(game_results, encoder);

  auto data_loader = torch::data::make_data_loader(
      std::move(data_set).map(Collate<ChessDataExample>(stack_examples)),
      batch_size);

  // Instantiate an SGD optimization algorithm to update our Net's parameters.
  // TODO: experiment with updating the learning rate.
  torch::optim::SGD optimizer(trained_net->parameters(), /*lr=*/0.01);

  unsigned num_checkpoint = 0;
  fs::path dir_path(checkpoint_dir);
  std::string dir_name;
  dir_name.reserve(32);

  for (size_t epoch = 1; epoch <= training_epochs; ++epoch) {
    size_t batch_index = 0;
    // Iterate the data loader to yield batches from the dataset.
    for (auto& batch : *data_loader) {
      // Reset gradients.
      optimizer.zero_grad();

      // Execute the model on the input data.
      auto [policy_pred, value_pred] = trained_net->forward(batch.data);

      // Get the target values from self play.
      auto& [policy_target, value_target] = batch.target;

      // TODO: compute loss for value
      torch::nn::MSELoss mse_loss;
      auto value_loss = mse_loss(value_pred, value_target);

      torch::nn::CrossEntropyLoss ce_loss;
      auto policy_loss = ce_loss(policy_pred, policy_target);

      // TODO: add L2 regularization.
      auto loss = value_loss + policy_loss;
      loss.backward();

      // Update the parameters based on the calculated gradients.
      optimizer.step();

      // Output the loss and checkpoint every batches.
      if (++batch_index % checkpoint_steps == 0) {
        std::cout << "Epoch: " << epoch
                  << " | Batch: " << batch_index
                  << " | Loss: " << loss.item<float>()
                  << std::endl;

        std::format_to(
            std::back_inserter(dir_name), "model-{:0>4}", num_checkpoint);
        trained_net->create_checkpoint(dir_path / dir_name);

        auto file_name = std::format("optim-{:0>4}.pt", num_checkpoint);
        torch::save(optimizer, dir_path / dir_name / file_name);

        // Clear the dir_name so we can reuse the buffer.
        dir_name.clear();
      }
    }
  }

  return trained_net;
}

// Runs the full training pipeline:
// 1. Generates training data by playing games of self-play.
// 2. Use the data from the games to train a new model.
// 3. Play a tournament between the new agent and the old agent.
// 4. If the the new agent has a sufficient winning percentage, then repeat 1-3
// with the new agent, otherwise repeat with the same agent.
void
Trainer::do_training() const
{
  throw std::runtime_error("Not implemented yet");
}

} // namespace blunder
