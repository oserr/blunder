#include "simple_game_builder.h"

#include <cstdint>
#include <memory>
#include <stdexcept>
#include <utility>

#include "alpha_zero_decoder.h"
#include "alpha_zero_encoder.h"
#include "alpha_zero_evaluator.h"
#include "blunder_player.h"
#include "mcts.h"
#include "net.h"
#include "player.h"
#include "tensor_decoder.h"
#include "tensor_encoder.h"

namespace blunder {

SimpleGameBuilder&
SimpleGameBuilder::set_white_net(std::shared_ptr<AlphaZeroNet> white_net)
{
  this->white_net = std::move(white_net);
  return *this;
}

SimpleGameBuilder&
SimpleGameBuilder::set_black_net(std::shared_ptr<AlphaZeroNet> black_net)
{
  this->black_net = std::move(black_net);
  return *this;
}

SimpleGameBuilder&
SimpleGameBuilder::set_net(std::shared_ptr<AlphaZeroNet> net)
{
  this->white_net = net;
  this->black_net = std::move(net);
  return *this;
}

SimpleGameBuilder&
SimpleGameBuilder::set_max_moves(unsigned max_moves)
{
  this->max_moves = max_moves;
  return *this;
}

SimpleGameBuilder&
SimpleGameBuilder::set_simulations(unsigned simulations)
{
  this->simulations = simulations;
  return *this;
}

SimpleGameBuilder&
SimpleGameBuilder::set_white_seed(std::uint64_t white_seed)
{
  this->white_seed = white_seed;
  return *this;
}

SimpleGameBuilder&
SimpleGameBuilder::set_black_seed(std::uint64_t black_seed)
{
  this->black_seed = black_seed;
  return *this;
}

SimpleGameBuilder&
SimpleGameBuilder::set_decoder(std::shared_ptr<TensorDecoder> decoder)
{
  this->decoder = std::move(decoder);
  return *this;
}

SimpleGameBuilder&
SimpleGameBuilder::set_encoder(std::shared_ptr<TensorEncoder> encoder)
{
  this->encoder = std::move(encoder);
  return *this;
}

SimpleGame
SimpleGameBuilder::build()
{
  if (not white_net)
    throw std::invalid_argument("white_net is null.");
  if (not black_net)
    throw std::invalid_argument("black_net is null.");
  if (not max_moves)
    throw std::invalid_argument("max_moves is zero.");
  if (not simulations)
    throw std::invalid_argument("simulations is zero.");

  if (not decoder)
    decoder = std::make_shared<AlphaZeroDecoder>();
  if (not encoder)
    encoder = std::make_shared<AlphaZeroEncoder>();

  auto wp = create_player(white_net, white_seed);
  auto bp = create_player(black_net, black_seed);

  return SimpleGame(std::move(wp), std::move(bp), max_moves);
}

std::unique_ptr<Player>
SimpleGameBuilder::create_player(
    std::shared_ptr<AlphaZeroNet> net,
    std::uint64_t seed)
{
  auto evaluator = std::make_shared<AlphaZeroEvaluator>(
          std::move(net), decoder, encoder);
  auto mcts = std::make_shared<Mcts>(
          std::move(evaluator), simulations, seed);
  return std::make_unique<BlunderPlayer>(std::move(mcts));
}

} // namespace blunder
