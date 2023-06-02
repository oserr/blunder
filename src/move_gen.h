#pragma once

#include <utility>
#include <vector>

#include "board_state.h"
#include "magics.h"
#include "move_generator.h"

namespace blunder {

// MoveGen implements MoveGenerator. It uses magic bitboards to generate moves
// for knights and bishops.
class MoveGen : public MoveGenerator {
public:
  // Initializes MoveGen with magic bitboards for bishops and rooks.
  MoveGen(MagicAttacks bmagics, MagicAttacks rmagics) noexcept
    : bmagics_(std::move(bmagics)),
      rmagics_(std::move(rmagics)) {}

  // Generates all possible moves for a king, including castling with the rook.
  // These may include moves that put the king in check. It is up to client to
  // determine which moves put the king in check.
  std::vector<Move>
  KingMoves(const BoardState& state) const override;

  // Generates all possible moves for queens on the board.
  std::vector<Move>
  QueenMoves(const BoardState& state) const override;

  // Generates all possible moves for rooks on the board, except for castling
  // moves.
  std::vector<Move>
  RookMoves(const BoardState& state) const override;

  // Generates all possible moves for bishops on the board.
  std::vector<Move>
  BishopMoves(const BoardState& state) const override;

  // Generates all possible moves for knights on the board.
  std::vector<Move>
  KnightMoves(const BoardState& state) const override;

  // Generates all possible moves for pawns on the board.
  std::vector<Move>
  PawnMoves(const BoardState& state) const override;

  // Generates all possible moves for all pieces on the board.
  std::vector<Move>
  AllMoves(const BoardState& state) const override;

private:
  // Generates all possible moves for a king, including castling with the rook.
  // These may include moves that put the king in check. It is up to client of
  // MoveGen to determine which moves put the king in check.
  void
  KingMoves(const BoardState& state, std::vector<Move>& moves) const;

  // Generates all possible moves for queens on the board.
  void
  QueenMoves(const BoardState& state, std::vector<Move>& moves) const;

  // Generates all possible moves for rooks on the board, except for castling
  // moves.
  void
  RookMoves(const BoardState& state, std::vector<Move>& moves) const;

  // Generates all possible moves for bishops on the board.
  void
  BishopMoves(const BoardState& state, std::vector<Move>& moves) const;

  // Generates all possible moves for knights on the board.
  void
  KnightMoves(const BoardState& state, std::vector<Move>& moves) const;

  // Generates all possible moves for pawns on the board.
  void
  PawnMoves(const BoardState& state, std::vector<Move>& moves) const;

  // Magic bitboards for bishops.
  MagicAttacks bmagics_;

  // Magic bitboards for rooks.
  MagicAttacks rmagics_;
};

} // namespace blunder
