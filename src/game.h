#pragma once

#include <vector>
#include <optional>

#include "board.h"
#include "color.h"
#include "search_result.h"

namespace blunder {

struct GameResult {
  Board game_start;
  std::vector<PlayResult> history;
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
