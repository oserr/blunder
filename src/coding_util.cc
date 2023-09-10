#include "coding_util.h"

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

// Maps a queen move to a number in the range [0, 55].
unsigned
encode_queen_move(int row_diff, int col_diff)
{
  assert(row_diff >= -7 and row_diff <= 7);
  assert(col_diff >= -7 and col_diff <= 7);
  assert(col_diff != 0 or row_diff != 0);

  if (row_diff < 0) {
    unsigned nrows = row_diff * -1;
    if (col_diff == 0)
      return nrows;
    else if (col_diff < 0)
      return 7 + nrows;
    else
      return 14 + nrows;
  }
  else if (row_diff == 0) {
    if (col_diff < 0)
      return 21 + col_diff * -1;
    else
      return 28 + col_diff;
  }
  else if (col_diff == 0)
    return 35 + row_diff;
  else if (col_diff < 0)
    return 42 + row_diff;
  else
    return 49 + row_diff;
}

} // namespace

EncodedMove
encode_move(Move mv)
{
  auto piece = mv.piece();
  auto [from_row, from_col] = row_col(mv.from());
  auto [to_row, to_col]  = row_col(mv.to());
  auto row_diff = to_row - from_row;
  auto col_diff = to_col - from_col;

  assert(row_diff != 0 or col_diff != 0);

  EncodedMove mv_code(from_row, from_col);

  switch (piece.type()) {
    case Type::None:
      throw std::invalid_argument("Move should have a piece.");
    case Type::Knight:
      mv_code.code = 56 + encode_knight_move(row_diff, col_diff);
      return mv_code;
    case Type::Pawn:
      if (mv.is_promo() and not mv.capture().is_queen()) {
        mv_code.code = 64 + encode_under_promo(col_diff, mv.capture());
        return mv_code;
      }
      [[fallthrough]];
    default:
      break;
  }

  mv_code.code = encode_queen_move(row_diff, col_diff);

  return mv_code;
}

} // namespace blunder
