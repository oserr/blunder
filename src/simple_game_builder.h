#pragma once

#include <cstdint>
#include <memory>

#include "net.h"
#include "player.h"
#include "simple_game.h"
#include "tensor_decoder.h"
#include "tensor_encoder.h"

namespace blunder {

class SimpleGameBuilder {
public:
  SimpleGameBuilder&
  set_white_net(std::shared_ptr<AlphaZeroNet> white_net);

  SimpleGameBuilder&
  set_black_net(std::shared_ptr<AlphaZeroNet> black_net);

  SimpleGameBuilder&
  set_net(std::shared_ptr<AlphaZeroNet> net);

  SimpleGameBuilder&
  set_max_moves(unsigned max_moves);

  SimpleGameBuilder&
  set_simulations(unsigned simulations);

  SimpleGameBuilder&
  set_white_seed(std::uint64_t white_seed);

  SimpleGameBuilder&
  set_black_seed(std::uint64_t black_seed);

  SimpleGameBuilder&
  set_decoder(std::shared_ptr<TensorDecoder> decoder);

  SimpleGameBuilder&
  set_encoder(std::shared_ptr<TensorEncoder> encoder);

  SimpleGame
  build();

private:

  std::unique_ptr<Player>
  create_player(std::shared_ptr<AlphaZeroNet> net, std::uint64_t seed);

  std::shared_ptr<AlphaZeroNet> white_net = nullptr;
  std::shared_ptr<AlphaZeroNet> black_net = nullptr;
  std::shared_ptr<TensorDecoder> decoder = nullptr;
  std::shared_ptr<TensorEncoder> encoder = nullptr;
  std::uint64_t white_seed = 0;
  std::uint64_t black_seed = 0;
  unsigned max_moves = 300;
  unsigned simulations = 800;
};

} // namespace blunder
