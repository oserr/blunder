#pragma once

#include <vector>

#include "board_state.h"
#include "move.h"

namespace blunder {

// MoveGenerator represents an interface for generating moves given a chess
// board position.
class MoveGenerator {
public:
  virtual MoveVec
  for_king(const Board& state) const = 0;

  virtual MoveVec
  for_queen(const Board& state) const = 0;

  virtual MoveVec
  for_rook(const Board& state) const = 0;

  virtual MoveVec
  for_bishop(const Board& state) const = 0;

  virtual MoveVec
  for_knight(const Board& state) const = 0;

  virtual MoveVec
  for_pawn(const Board& state) const = 0;

  virtual MoveVec
  for_all(const Board& state) const = 0;
};

} // namespace blunder
