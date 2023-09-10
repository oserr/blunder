#include "encoding.h"

#include <cassert>
#include <stdexcept>

#include "pieces.h"
#include "square.h"

namespace blunder {
namespace {

// Maps a knight move to a number in range [0, 7].
unsigned
encode_knight_move(int row_diff, int col_diff)
{
  assert(row_diff >= -2 and row_diff <= 2);
  assert(col_diff >= -2 and col_diff <= 2);
  assert(row_diff != col_diff);

  if (row_diff < 0 and col_diff < 0) {
    // Lower left quadrant.
    return row_diff == -2 ? 0 : 1;
  } else if (row_diff > 0 and col_diff < 0) {
    // Upper left quadrant.
    return row_diff == 1 ? 2 : 3;
  } else if (row_diff > 0 and col_diff > 0) {
    // Upper right quadrant.
    return row_diff == 2 ? 4 : 5;
  } else {
    // Lower right quadrant.
    return row_diff == -1 ? 6 : 7;
  }
}

// Maps an underpromotion move to a number in the range [0, 8].
unsigned
encode_under_promo(int col_diff, Piece piece)
{
  assert(col_diff >= -1 and col_diff <= 1);

  unsigned code = 0;

  switch (piece.type()) {
    case Type::Rook:
      break;
    case Type::Bishop:
      code += 3;
      break;
    case Type::Knight:
      code += 6;
      break;
    default:
      throw std::invalid_argument("Invalid piece for under promotion.");
  }

  if (col_diff == 0)
    code += 1;
  else if (col_diff == 1)
    code += 2;

  return code;
}

} // namespace

unsigned
encode_move(Move mv)
{
  auto piece = mv.piece();
  auto [from_row, from_col] = row_col(mv.from());
  auto [to_row, to_col]  = row_col(mv.to());
  auto row_diff = to_row - from_row;
  auto col_diff = to_col - from_col;

  assert(row_diff != 0 or col_diff != 0);

  switch (piece.type()) {
    case Type::None:
      throw std::invalid_argument("Move should have a piece.");
    case Type::Knight:
      return 56 + encode_knight_move(row_diff, col_diff);
    case Type::Pawn:
      if (mv.is_promo() and not mv.capture().is_queen())
        return 64 + encode_under_promo(col_diff, mv.capture());
      [[fallthrough]];
    default:
      break;
  }

  // If we are here then we are simply encoding a queen move.

  return 0;
}

} // namespace blunder
