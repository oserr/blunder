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
  for_king(const BoardState& state) const = 0;

  virtual MoveVec
  for_queen(const BoardState& state) const = 0;

  virtual MoveVec
  for_rook(const BoardState& state) const = 0;

  virtual MoveVec
  for_bishop(const BoardState& state) const = 0;

  virtual MoveVec
  for_knight(const BoardState& state) const = 0;

  virtual MoveVec
  for_pawn(const BoardState& state) const = 0;

  virtual MoveVec
  for_all(const BoardState& state) const = 0;
};

} // namespace blunder
