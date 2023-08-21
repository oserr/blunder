#include "simple_game.h"

#include <optional>
#include <utility>

#include "board.h"
#include "move.h"

namespace blunder {

GameResult
SimpleGame::play()
{
  std::vector<Board> history;
  history.emplace_back(Board::new_board());

  while (not history.back().is_terminal()) {
    const auto& board = history.back();
    auto next_board = board.is_white_next()
       ? wplayer->make_move(history)
       : bplayer->make_move(history);
    history.push_back(std::move(next_board));
  }

  const auto& board = history.back();
  std::optional<Color> winner;
  if (board.is_mate())
    winner = board.is_white_next() ? Color::Black : Color::White;

  return GameResult{
    .moves = std::move(history),
    .winner = winner};
}

} // namespace blunder
