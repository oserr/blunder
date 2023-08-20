#include "terminal_player.h"

namespace blunder {

// TODO: implement make_move.
Board
TerminalPlayer::make_move(std::span<const Board> boards)
{
  (void)boards;
  return Board::new_board();
}

} // namespace blunder
