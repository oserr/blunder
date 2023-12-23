#pragma once

#include "trainer.h"

namespace blunder {

class TrainerBuilder {
public:
  TrainerBuilder&
  set_training_sessions(unsigned training_sessions)
  {
    trainer.training_sessions = training_sessions;
    return *this;
  }

  TrainerBuilder&
  set_training_games(unsigned training_games)
  {
    trainer.training_games = training_games;
    return *this;
  }

  TrainerBuilder&
  set_training_epochs(unsigned training_epochs)
  {
    trainer.training_epochs = training_epochs;
    return *this;
  }

  TrainerBuilder&
  set_tournament_games(unsigned tournament_games)
  {
    trainer.tournament_games = tournament_games;
    return *this;
  }

  TrainerBuilder&
  set_min_win_rate(float min_win_rate)
  {
    trainer.min_win_rate = min_win_rate;
    return *this;
  }

  TrainerBuilder&
  set_checkpoint_steps(unsigned checkpoint_steps)
  {
    trainer.checkpoint_steps = checkpoint_steps;
    return *this;
  }

  TrainerBuilder&
  set_batch_size(unsigned batch_size)
  {
    trainer.batch_size = batch_size;
    return *this;
  }

  TrainerBuilder&
  set_max_moves_per_game(unsigned max_moves_per_game)
  {
    trainer.max_moves_per_game = max_moves_per_game;
    return *this;
  }

  TrainerBuilder&
  set_checkpoint_dir(std::string checkpoint_dir)
  {
    trainer.checkpoint_dir = checkpoint_dir;
    return *this;
  }

  TrainerBuilder&
  set_champion_net(std::shared_ptr<AlphaZeroNet> champion)
  {
    trainer.champion = std::move(champion);
    return *this;
  }

  TrainerBuilder&
  set_decoder(std::shared_ptr<TensorDecoder> decoder)
  {
    trainer.decoder = std::move(decoder);
    return *this;
  }

  TrainerBuilder&
  set_encoder(std::shared_ptr<TensorEncoder> encoder)
  {
    trainer.encoder = std::move(encoder);
    return *this;
  }

  Trainer
  build();

private:
  Trainer trainer;
};

} // namespace blunder
