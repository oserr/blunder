#include "move_gen.h"

#include <vector>

#include "board_state.h"
#include "move.h"

namespace blunder {

std::vector<Move>
MoveGen::KingMoves(const BoardState& state) const
{
  return std::vector<Move>();
}

// Generates all possible moves for queens on the board.
std::vector<Move>
MoveGen::QueenMoves(const BoardState& state) const
{
  return std::vector<Move>();
}

std::vector<Move>
MoveGen::RookMoves(const BoardState& state) const
{
  return std::vector<Move>();
}

std::vector<Move>
MoveGen::BishopMoves(const BoardState& state) const
{
  return std::vector<Move>();
}

std::vector<Move>
MoveGen::KnightMoves(const BoardState& state) const
{
  return std::vector<Move>();
}

std::vector<Move>
MoveGen::PawnMoves(const BoardState& state) const
{
  return std::vector<Move>();
}

std::vector<Move>
MoveGen::AllMoves(const BoardState& state) const
{
  return std::vector<Move>();
}

} // namespace blunder
