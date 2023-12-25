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
  game_result.moves.reserve(max_moves);
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

    const auto& pr = game_result.moves.emplace_back(std::move(play_result));
    const auto& next_board = pr.best.board;

    if (verbose) {
      std::cout << "move " << move_num++ << " -> "
                << *next_board.last_move()
                << "  value=" << pr.value << std::endl;
    }

    game_path.push(next_board);
  }

  const auto& board = game_path.fast_back();
  if (board.is_mate())
    game_result.winner = board.is_white_next() ? Color::Black : Color::White;

  return game_result;
}

} // namespace blunder
