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
  for_king(const BoardState& state) const override;

  // Generates all possible moves for queens on the board.
  std::vector<Move>
  for_queen(const BoardState& state) const override;

  // Generates all possible moves for rooks on the board, except for castling
  // moves.
  std::vector<Move>
  for_rook(const BoardState& state) const override;

  // Generates all possible moves for bishops on the board.
  std::vector<Move>
  for_bishop(const BoardState& state) const override;

  // Generates all possible moves for knights on the board.
  std::vector<Move>
  for_knight(const BoardState& state) const override;

  // Generates all possible moves for pawns on the board.
  std::vector<Move>
  for_pawn(const BoardState& state) const override;

  // Generates all possible moves for all pieces on the board.
  std::vector<Move>
  for_all(const BoardState& state) const override;

private:
  // Generates all possible moves for a king, including castling with the rook.
  // These may include moves that put the king in check. It is up to client of
  // MoveGen to determine which moves put the king in check.
  void
  for_king(const BoardState& state, std::vector<Move>& moves) const;

  // Generates all possible moves for queens on the board.
  void
  for_queen(const BoardState& state, std::vector<Move>& moves) const;

  // Generates all possible moves for rooks on the board, except for castling
  // moves.
  void
  for_rook(const BoardState& state, std::vector<Move>& moves) const;

  // Generates all possible moves for bishops on the board.
  void
  for_bishop(const BoardState& state, std::vector<Move>& moves) const;

  // Generates all possible moves for knights on the board.
  void
  for_knight(const BoardState& state, std::vector<Move>& moves) const;

  // Generates all possible moves for pawns on the board.
  void
  for_pawn(const BoardState& state, std::vector<Move>& moves) const;

  // Magic bitboards for bishops.
  MagicAttacks bmagics_;

  // Magic bitboards for rooks.
  MagicAttacks rmagics_;
};

} // namespace blunder
