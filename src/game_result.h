#pragma once

#include <vector>
#include <optional>

#include "board.h"
#include "color.h"
#include "search_result.h"

namespace blunder {

struct GameResult {
  Board game_start;
  std::vector<SearchResult> moves;
  std::optional<Color> winner;

  // Maximum nodes expanded in any one run.
  unsigned max_nodes_expanded = 0;

  // The average number of nodes expanded per search.
  float avg_nodes_expanded = 0;

  // The maximum depth of a branch explored during search.
  unsigned max_depth = 0;

  // The average depth for each search run.
  float avg_depth = 0;

  // Average number of milliseconds per evaluation of node for the entire game.
  float millis_per_eval = 0;

  // The average number of milliseconds per search for the entire game.
  float millis_per_search = 0;
};

} // namespace blunder
