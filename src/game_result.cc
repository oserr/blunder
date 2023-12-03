#include "game_result.h"

#include <format>
#include <iterator>
#include <stdexcept>
#include <utility>

namespace blunder {

std::string
GameStats::dbg() const {
  std::string buffer;
  buffer.reserve(256);
  std::format_to(std::back_inserter(buffer),
    R"(
    GameStats[
        max_nodes_expanded={},
        avg_nodes_expanded={:.3f}
        max_nodes_visited={}
        avg_nodes_visited={:.3f}
        max_depth={}
        avg_depth={:.3f}
        millis_per_eval={:.3f}
        millis_per_search={:.3f}
    ])",
  max_nodes_expanded,
  avg_nodes_expanded,
  max_nodes_visited,
  avg_nodes_visited,
  max_depth,
  avg_depth,
  millis_per_eval,
  millis_per_search);

  return buffer;
}

GameStats
GameResult::stats() const {
  GameStats game_stats;

  unsigned total_depth = 0;
  unsigned total_nodes_expanded = 0;
  unsigned total_nodes_visited = 0;
  float total_millis_per_eval = 0;
  float total_millis_per_search = 0;

  for (const auto& sr : moves) {
    total_depth += sr.depth;
    game_stats.max_depth = std::max(game_stats.max_depth, sr.depth);

    total_nodes_expanded += sr.nodes_expanded;
    total_nodes_visited += sr.nodes_visited;

    game_stats.max_nodes_expanded = std::max(
        game_stats.max_nodes_expanded,
        sr.nodes_expanded);

    game_stats.max_nodes_visited = std::max(
        game_stats.max_nodes_visited,
        sr.nodes_visited);

    total_millis_per_eval += sr.millis_per_eval;
    total_millis_per_search += sr.millis_search_time;
  }

  auto n = moves.size();

  if (not n)
    throw std::runtime_error("Cannot compute game stats without moves.");

  game_stats.avg_nodes_expanded = static_cast<float>(total_nodes_expanded) / n;
  game_stats.avg_nodes_visited = static_cast<float>(total_nodes_visited) / n;
  game_stats.avg_depth = static_cast<float>(total_depth) / n;
  game_stats.millis_per_eval = total_millis_per_eval / n;
  game_stats.millis_per_search = total_millis_per_search / n;

  return game_stats;
}

} // namespace blunder
