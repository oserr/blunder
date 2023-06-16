#include "board_state.h"

namespace blunder {
namespace {

BitBoard
AllMask(const PieceSet& pieces) noexcept
{
  BitBoard mask = 0ull;
  for (auto piece : pieces)
    mask |= piece;
  return mask;
}
} // namespace

void
BoardState::SetAllMine() noexcept
{ all_mine = AllMask(mine); }

void
BoardState::SetAllOther() noexcept
{ all_other = AllMask(other); }

} // namespace blunder
