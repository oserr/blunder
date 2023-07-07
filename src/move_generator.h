#pragma once

#include <vector>

#include "board_state.h"
#include "move.h"

namespace blunder {

// MoveGenerator represents an interface for generating moves given a chess
// board position.
class MoveGenerator {
public:
  virtual std::vector<Move>
  for_king(const BoardState& state) const = 0;

  virtual std::vector<Move>
  for_queen(const BoardState& state) const = 0;

  virtual std::vector<Move>
  for_rook(const BoardState& state) const = 0;

  virtual std::vector<Move>
  for_bishop(const BoardState& state) const = 0;

  virtual std::vector<Move>
  for_knight(const BoardState& state) const = 0;

  virtual std::vector<Move>
  for_pawn(const BoardState& state) const = 0;

  virtual std::vector<Move>
  for_all(const BoardState& state) const = 0;
};

} // namespace blunder
