#pragma once

#include <cassert>
#include <memory>
#include <string_view>
#include <utility>
#include <vector>

#include "game_result.h"
#include "net.h"
#include "search.h"
#include "search_result.h"

namespace blunder {

class Trainer {
public:
  // Initializes the Trainer with an initial Search agent.
  // TODO: create a TrainingBuilder to build the Trainer agent.
  Trainer(
      std::shared_ptr<Search> mcts,
      unsigned training_games,
      float min_win_rate)
    : white_mcts(std::move(mcts)),
      black_mcts(white_mcts),
      training_games(training_games),
      tournament_games(min_win_rate),
      min_win_rate(min_win_rate)
  {
    assert(this->white_mcts);
  }

  // Plays the training games.
  std::vector<GameResult>
  play_training_games() const;

  // Plays the tournament games.
  std::vector<GameResult>
  play_tournament_games() const;

  // Trains the model.
  std::shared_ptr<AlphaZeroNet>
  train_model() const;

  // Runs the full training pipeline.
  void
  do_training() const;

private:
  std::shared_ptr<Search> white_mcts;
  std::shared_ptr<Search> black_mcts;

  // The total number of training games to use for each training session.
  unsigned training_games = 0;

  // The total number of tournament games to play between two models.
  unsigned tournament_games = 0;

  // The minimum win rate required for a new model to replace the current model.
  float min_win_rate = 0.0;

  // The number of steps to take before creating a checkpoint for the model.
  unsigned checkpoint_steps = 100;

  // The number of steps to take before creating a checkpoint for the model.
  unsigned batch_size = 64;

  // TODO: Add other configuration parameters, e.g. learning rate, etc.
};

} // namespace  blunder
