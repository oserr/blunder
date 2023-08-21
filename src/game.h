#pragma once

#include <vector>
#include <optional>

#include "board.h"
#include "color.h"

namespace blunder {

struct GameResult {
  std::vector<Board> moves;
  std::optional<Color> winner;
};

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
