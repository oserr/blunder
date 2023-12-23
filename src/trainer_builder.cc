#include "trainer_builder.h"

#include <stdexcept>

#include "alpha_zero_decoder.h"
#include "alpha_zero_encoder.h"
#include "net.h"

namespace blunder {

Trainer
TrainerBuilder::build()
{
    if (not trainer.training_sessions)
      throw std::invalid_argument("training_sessions must be non-zero.");

    if (not trainer.training_epochs)
      throw std::invalid_argument("training_epochs must be non-zero.");

    if (not trainer.training_games)
      throw std::invalid_argument("training_games must be non-zero.");

    if (not trainer.tournament_games)
      throw std::invalid_argument("tournament_games must be non-zero.");

    if (trainer.min_win_rate < .51 or trainer.min_win_rate >= 1.0)
      throw std::invalid_argument("min_win_rate must be in range [0.51,1.0).");

    if (not trainer.checkpoint_steps)
      throw std::invalid_argument("checkpoint_steps must be non-zero.");

    if (not trainer.batch_size)
      throw std::invalid_argument("batch_size must be non-zero.");

    if (not trainer.max_moves_per_game)
      throw std::invalid_argument("max_moves_per_game must be non-zero.");

    if (trainer.checkpoint_dir.empty())
      trainer.checkpoint_dir = "checkpoints";

    if (not trainer.champion)
      trainer.champion = std::make_shared<AlphaZeroNet>();

    if (not trainer.decoder)
      trainer.decoder = std::make_shared<AlphaZeroDecoder>();

    if (not trainer.encoder)
      trainer.encoder = std::make_shared<AlphaZeroEncoder>();

    return trainer;
}

} // namespace blunder
