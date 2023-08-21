#pragma once

#include <memory>
#include <utility>

#include "game.h"
#include "player.h"

namespace blunder {

// Implements a simple chess game between two players.
class SimpleGame : public Game {
public:
  SimpleGame(
      std::unique_ptr<Player> white_player,
      std::unique_ptr<Player> black_player)
    : wplayer(std::move(white_player)),
      bplayer(std::move(black_player)) {}

  GameResult
  play() override;

private:
  // Player for white pieces.
  std::unique_ptr<Player> wplayer;

  // Player for black pieces.
  std::unique_ptr<Player> bplayer;
};

} // namespace blunder
