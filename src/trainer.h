#pragma once

#include <cassert>
#include <memory>
#include <span>
#include <string>
#include <utility>
#include <vector>

#include "net.h"
#include "game_result.h"
#include "net.h"
#include "search.h"
#include "search_result.h"
#include "tensor_decoder.h"
#include "tensor_encoder.h"

namespace blunder {

struct MatchStats {
  unsigned champion_wins = 0;
  unsigned contender_wins = 0;
  unsigned draws = 0;
};

class Trainer {
public:
  // Runs the full training pipeline.
  void
  train() const;

private:
  // Make this private to force clients to use the builder.
  // TODO: create a TrainingBuilder to build the Trainer agent.
  Trainer();

  // Plays the training games.
  std::vector<GameResult>
  play_training_games(std::shared_ptr<AlphaZeroNet> net) const;

  // Plays the tournament games.
  MatchStats
  play_tournament(std::shared_ptr<AlphaZeroNet> contender) const;

  // Creates a new version of the model by training the current model.
  // @param game_results A collection of games for use as training data.
  // @param net The current version of the model.
  // @return A new version of the model that is trained on game_results.
  std::shared_ptr<AlphaZeroNet>
  train_model(
      std::span<const GameResult> game_results,
      const AlphaZeroNet& net) const;

  // The total number of training sessions. Each session consists of a round of
  // training games, model training, and tournament games.
  unsigned training_sessions = 10;

  // The total number of training epochs to use during model training.
  unsigned training_epochs = 10;

  // The total number of training games to use for each training session.
  unsigned training_games = 1000;

  // The total number of tournament games to play between two models.
  unsigned tournament_games = 400;

  // The minimum win rate required for a new model to replace the current model.
  float min_win_rate = 0.55;

  // The number of steps to take before creating a checkpoint for the model.
  unsigned checkpoint_steps = 100;

  // The number of steps to take before creating a checkpoint for the model.
  unsigned batch_size = 32;

  // Maximum number of moves per game before game is drawn.
  unsigned max_moves_per_game = 300;

  // The directory where checkpoints are created.
  std::string checkpoint_dir;

  std::shared_ptr<TensorDecoder> decoder = nullptr;
  std::shared_ptr<TensorEncoder> encoder = nullptr;

  // The current champion network.
  mutable std::shared_ptr<AlphaZeroNet> champion = nullptr;

  friend class TrainerBuilder;
  // TODO: Add other configuration parameters, e.g. learning rate, etc.
};

} // namespace  blunder
