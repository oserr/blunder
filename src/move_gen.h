#pragma once

#include <utility>

#include "board.h"
#include "magic_attacks.h"
#include "move_generator.h"

namespace blunder {

// MoveGen implements MoveGenerator. It uses magic bitboards to generate moves
// for knights and bishops.
class MoveGen : public MoveGenerator {
public:
  // Initializes MoveGen with magic bitboards for bishops and rooks.
  MoveGen(MagicAttacks bmagics, MagicAttacks rmagics) noexcept
    : bmagics(std::move(bmagics)),
      rmagics(std::move(rmagics)) {}

  // Generates all possible moves for a king, including castling with the rook.
  // These may include moves that put the king in check. It is up to client to
  // determine which moves put the king in check.
  MoveVec
  for_king(const Board& state) const override;

  // Generates all possible moves for queens on the board.
  MoveVec
  for_queen(const Board& state) const override;

  // Generates all possible moves for rooks on the board, except for castling
  // moves.
  MoveVec
  for_rook(const Board& state) const override;

  // Generates all possible moves for bishops on the board.
  MoveVec
  for_bishop(const Board& state) const override;

  // Generates all possible moves for knights on the board.
  MoveVec
  for_knight(const Board& state) const override;

  // Generates all possible moves for pawns on the board.
  MoveVec
  for_pawn(const Board& state) const override;

  // Generates all possible moves for all pieces on the board.
  MoveVec
  for_all(const Board& state) const override;

private:
  // Generates all possible moves for a king, including castling with the rook.
  // These may include moves that put the king in check. It is up to client of
  // MoveGen to determine which moves put the king in check.
  void
  for_king(const Board& state, MoveVec& moves) const;

  // Generates all possible moves for queens on the board.
  void
  for_queen(const Board& state, MoveVec& moves) const;

  // Generates all possible moves for rooks on the board, except for castling
  // moves.
  void
  for_rook(const Board& state, MoveVec& moves) const;

  // Generates all possible moves for bishops on the board.
  void
  for_bishop(const Board& state, MoveVec& moves) const;

  // Generates all possible moves for knights on the board.
  void
  for_knight(const Board& state, MoveVec& moves) const;

  // Generates all possible moves for pawns on the board.
  void
  for_pawn(const Board& state, MoveVec& moves) const;

  // Magic bitboards for bishops.
  MagicAttacks bmagics;

  // Magic bitboards for rooks.
  MagicAttacks rmagics;
};

} // namespace blunder
