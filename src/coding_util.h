#pragma once

#include <cassert>

#include "move.h"

namespace blunder {

// Represents the the code for a move.
struct EncodedMove {
  int row = 0;
  int col = 0;
  int code = 0;

  EncodedMove(int row, int col)
    : row(row),
      col(col)
  {
    assert(row >= 0 and row <= 7);
    assert(col >= 0 and col <= 7);
  }
};

// Encodes |mv| as a triple of
// - the row where the piece is moving from.
// - the column where the piece is moving from.
// - a code to represent the move.
EncodedMove
encode_move(Move mv);

} // namespace blunder
