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

  // Maximum nodes expanded in any one run.
  std::uint64_t max_nodes_expanded = 0;

  // The average number of nodes expanded per search.
  std::uint64_t avg_nodes_expanded = 0;

  // The maximum depth of a branch explored during search.
  unsigned max_depth = 0;

  // The average depth for each search run.
  unsigned avg_depth = 0;

  // Average number of milliseconds per evaluation of node for the entire game.
  float millis_per_eval = 0;

  // The average number of milliseconds per search for the entire game.
  float millis_per_search = 0;
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
