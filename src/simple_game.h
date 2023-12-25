#pragma once

#include <memory>
#include <utility>

#include "game.h"
#include "player.h"

namespace blunder {

// TODO: modify or extend interface to play games from random positions.
// Implements a simple chess game between two players.
class SimpleGame : public Game {
public:
  SimpleGame(
      std::unique_ptr<Player> white_player,
      std::unique_ptr<Player> black_player,
      unsigned max_moves = 300)
    : wplayer(std::move(white_player)),
      bplayer(std::move(black_player)),
      max_moves(max_moves)
  {
    if (not max_moves)
      throw std::invalid_argument("max_moves cannot be zero");
  }

  GameResult
  play() override;

  void
  flip_colors()
  { wplayer.swap(bplayer); }

private:
  // Player for white pieces.
  std::unique_ptr<Player> wplayer;

  // Player for black pieces.
  std::unique_ptr<Player> bplayer;

  unsigned max_moves;
  bool verbose = false;

  friend class SimpleGameBuilder;
};

} // namespace blunder
