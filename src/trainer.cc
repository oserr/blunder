#include "trainer.h"

#include <memory>
#include <stdexcept>
#include <utility>
#include <vector>

#include "alpha_zero_encoder.h"
#include "chess_data_set.h"
#include "game_result.h"
#include "net.h"

#include <torch/torch.h>

namespace blunder {

// Plays the training games.
std::vector<GameResult>
Trainer::play_training_games() const
{
  throw std::runtime_error("Not implemented yet");
}

// Plays the tournament games.
std::vector<GameResult>
Trainer::play_tournament_games() const
{
  throw std::runtime_error("Not implemented yet");
}

// Trains the model.
std::shared_ptr<AlphaZeroNet>
Trainer::train_model() const
{
  // TODO: don't create a new net, but used the one to generate the training
  // data.
  auto net = std::make_shared<AlphaZeroNet>();

  // Create the encoder.
  auto encoder = std::make_shared<AlphaZeroEncoder>();

  std::vector<GameResult> game_results;
  ChessDataSet data_set(game_results, encoder);

  // Create a multi-threaded data loader for the MNIST dataset.
  auto data_loader = torch::data::make_data_loader(
      //torch::data::datasets::MNIST("./data").map(
      //    torch::data::transforms::Stack<>()),
      std::move(data_set),
      /*batch_size=*/64);

  // Instantiate an SGD optimization algorithm to update our Net's parameters.
  torch::optim::SGD optimizer(net->parameters(), /*lr=*/0.01);

  for (size_t epoch = 1; epoch <= 10; ++epoch) {
    size_t batch_index = 0;
    // Iterate the data loader to yield batches from the dataset.
    for (auto& batch : *data_loader) {
      // TODO: define a function to stack Elements when we create the data
      // loader, so we can operate on the whole batch rather than individual
      // examples. For now do this to get it to compile.
      for (auto& example : batch) {
        // Reset gradients.
        optimizer.zero_grad();

        // Execute the model on the input data.
        auto [policy_pred, value_pred] = net->forward(example.data);

        // Get the target values from self play.
        auto& [policy_target, value_target] = example.target;

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

          // TODO: create checkpoint correctly. The name needs to be different
          // for each checkpoint.
          torch::save(net, "net.pt");
        }
      }
    }
  }
  return net;
}

// Runs the full training pipeline.
void
Trainer::do_training() const
{
  throw std::runtime_error("Not implemented yet");
}

} // namespace blunder
