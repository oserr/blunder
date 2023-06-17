#include "board_state.h"

#include "bitboard.h"
#include "pieces.h"

namespace blunder {

BitBoard
AllMask(const PieceSet& pieces) noexcept
{
  BitBoard mask = 0ull;
  for (auto piece : pieces)
    mask |= piece;
  return mask;
}

BoardState
NewBoardState() noexcept
{
  BoardState state;

  state.mine[Uint8(Piece::King)] = kWhiteKing;
  state.mine[Uint8(Piece::Queen)] = kWhiteQueen;
  state.mine[Uint8(Piece::Rook)] = kWhiteRooks;
  state.mine[Uint8(Piece::Bishop)] = kWhiteBishops;
  state.mine[Uint8(Piece::Knight)] = kWhiteKnights;
  state.mine[Uint8(Piece::Pawn)] = kWhitePawns;

  state.other[Uint8(Piece::King)] = kBlackKing;
  state.other[Uint8(Piece::Queen)] = kBlackQueen;
  state.other[Uint8(Piece::Rook)] = kBlackRooks;
  state.other[Uint8(Piece::Bishop)] = kBlackBishops;
  state.other[Uint8(Piece::Knight)] = kBlackKnights;
  state.other[Uint8(Piece::Pawn)] = kBlackPawns;

  state.all_mine = kWhitePieces;
  state.all_other = kBlackPieces;

  state.en_passant = 0;
  state.en_passant_file = 0;

  state.wk_castle = 0;
  state.wq_castle = 0;
  state.bk_castle = 0;
  state.bq_castle = 0;

  return state;
}

} // namespace blunder
