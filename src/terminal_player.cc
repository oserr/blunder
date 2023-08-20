#include "terminal_player.h"

#include <iostream>

namespace blunder {

// TODO: implement make_move.
Board
TerminalPlayer::make_move(std::span<const Board> boards)
{
  if (boards.empty())
    throw std::invalid_argument("Boards should not be empty.");
  (void)boards;
  return Board::new_board();
}

} // namespace blunder
