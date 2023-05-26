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
  KingMoves(const BoardState& state) const = 0;

  virtual std::vector<Move>
  QueenMoves(const BoardState& state) const = 0;

  virtual std::vector<Move>
  RookMoves(const BoardState& state) const = 0;

  virtual std::vector<Move>
  BishopMoves(const BoardState& state) const = 0;

  virtual std::vector<Move>
  KnightMoves(const BoardState& state) const = 0;

  virtual std::vector<Move>
  PawnMoves(const BoardState& state) const = 0;

  virtual std::vector<Move>
  AllMoves(const BoardState& state) const = 0;
};

} // namespace blunder
