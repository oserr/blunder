#pragma once

#include <optional>
#include <string>
#include <vector>

#include "board.h"
#include "color.h"
#include "game_winner.h"
#include "search_result.h"

namespace blunder {

struct GameStats {
  // Maximum nodes expanded in any one run.
  unsigned max_nodes_expanded = 0;

  // The average number of nodes expanded per search.
  float avg_nodes_expanded = 0;

  // The average number of nodes visited per search.
  unsigned max_nodes_visited = 0;

  // The average number of nodes visited per search.
  float avg_nodes_visited = 0;

  // The maximum depth of a branch explored during search.
  unsigned max_depth = 0;

  // The average depth for each search run.
  float avg_depth = 0;

  // Average number of milliseconds per evaluation of node for the entire game.
  float millis_per_eval = 0;

  // The average number of milliseconds per search for the entire game.
  float millis_per_search = 0;

  GameWinner game_winner = GameWinner::None;

  // Creats a debug string.
  std::string
  dbg() const;
};

struct GameResult {
  Board game_start;
  std::vector<SearchResult> moves;
  // TODO: replace this with GameWinner.
  std::optional<Color> winner;

  GameStats
  stats() const;
};

} // namespace blunder
