#pragma once

#include "board.h"
#include "color.h"
#include "game_result.h"

namespace blunder {

// An interface for a chess game.
class Game {
public:
  // Default ctor.
  virtual ~Game() = default;

  // Plays the game through.
  virtual GameResult
  play() = 0;
};

} // namespace blunder
