#include "simple_game.h"

#include <iostream>
#include <optional>
#include <utility>

#include "board.h"
#include "board_path.h"
#include "move.h"
#include "search_result.h"

namespace blunder {

GameResult
SimpleGame::play()
{
  GameResult game_result;
  // TODO: get the 300 programmatically.
  game_result.history.reserve(300);
  game_result.game_start = Board::new_board();
  GameBoardPath game_path;
  game_path.push(game_result.game_start);

  int move_num = 1;

  while (not game_path.fast_back().is_terminal()) {
    if (game_path.is_full())
      break;

    const auto& board = game_path.fast_back();

    auto play_result = board.is_white_next()
       ? wplayer->make_move(game_path)
       : bplayer->make_move(game_path);

    const auto& pr = game_result.history.emplace_back(std::move(play_result));
    const auto& next_board = pr.play_move.board;

    std::cout << "move " << move_num++ << " -> "
              << *next_board.last_move()
              << "  value=" << pr.value << std::endl;

    game_path.push(next_board);
  }

  const auto& board = game_path.fast_back();
  if (board.is_mate())
    game_result.winner = board.is_white_next() ? Color::Black : Color::White;

  // Compute other statistics.
  unsigned total_depth = 0;
  unsigned total_nodes_expanded = 0;
  float total_millis_per_eval = 0;
  float total_millis_per_search = 0;

  for (const auto& presult : game_result.history) {
    total_depth += presult.depth;
    game_result.max_depth = std::max(game_result.max_depth, presult.depth);

    total_nodes_expanded += presult.num_expanded;
    game_result.max_nodes_expanded = std::max(
        game_result.max_nodes_expanded,
        presult.num_expanded);

    total_millis_per_eval += presult.millis_per_eval;
    total_millis_per_search += presult.millis_search_time;
  }

  unsigned nplays = game_result.history.size();
  assert(nplays and "nplays cannot be zero.");
  game_result.avg_nodes_expanded = static_cast<float>(total_nodes_expanded) / nplays;
  game_result.avg_depth = static_cast<float>(total_depth) / nplays;
  game_result.millis_per_eval = total_millis_per_eval / nplays;
  game_result.millis_per_search = total_millis_per_search / nplays;

  return game_result;
}

} // namespace blunder
