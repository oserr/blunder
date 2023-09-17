#include "blunder_player.h"

namespace blunder {

PlayResult
BlunderPlayer::make_move(const GameBoardPath& boards)
{
  auto board_path = EvalBoardPath::rev(boards);
  auto result = mcts->run(board_path);
  return PlayResult::take_from(result);
}

} // namespace blunder
