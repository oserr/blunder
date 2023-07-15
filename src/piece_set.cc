#include "piece_set.h"

namespace blunder {

Piece
PieceSet::find_type(BitBoard bb) const noexcept
{
  assert(bb.has_single_bit()
        and "BitBoard bb should only have 1 bit set.");

  for (std::uint8_t i = 0; i < pieces.size(); ++i) {
    if (bb & pieces[i])
      return Piece::from_int(i);
  }
  return Piece::none();
}

PieceSet
PieceSet::init_white() noexcept
{
  PieceSet pieces;
  pieces.pieces = {
    kWhiteKing,
    kWhiteQueen,
    kWhiteRooks,
    kWhiteBishops,
    kWhiteKnights,
    kWhitePawns,
  };
  pieces.all_bits = kWhitePieces;
  return pieces;
}

PieceSet
PieceSet::init_black() noexcept
{
  PieceSet pieces;
  pieces.pieces = {
    kBlackKing,
    kBlackQueen,
    kBlackRooks,
    kBlackBishops,
    kBlackKnights,
    kBlackPawns,
  };
  pieces.all_bits = kBlackPieces;
  return pieces;
}

bool
PieceSet::is_valid() const noexcept
{
  return king().count() == 1
     and queen().count() <= 9
     and rook().count() <= 10
     and bishop().count() <= 10
     and knight().count() <= 10
     and pawn().count() <= 10
     and full_set().count() <= 16;
}

} // namespace blunder
