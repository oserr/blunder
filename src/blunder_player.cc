#include "blunder_player.h"

namespace blunder {

SearchResult
BlunderPlayer::make_move(const GameBoardPath& boards)
{
  auto board_path = EvalBoardPath::rev(boards);
  return mcts->run(board_path);
}

} // namespace blunder
