#include "piece_set.h"

#include <optional>

namespace blunder {

PieceSet&
PieceSet::set_bit(Type type, unsigned index) noexcept
{
  get_mut(type).set_bit(index);
  all_bits.set_bit(index);
  return *this;
}

PieceSet&
PieceSet::clear_bit(Type type, unsigned index) noexcept
{
  get_mut(type).clear_bit(index);
  all_bits.clear_bit(index);
  return *this;
}

PieceSet&
PieceSet::update_bit(Type type, unsigned from, unsigned to)
{
  get_mut(type).update_bit(from, to);
  all_bits.update_bit(from, to);
  return *this;
}

BitBoard
PieceSet::get(Piece piece) const
{
  switch (piece.type()) {
    case Type::King: return king();
    case Type::Queen: return queen();
    case Type::Rook: return rook();
    case Type::Bishop: return bishop();
    case Type::Knight: return knight();
    default: return pawn();
  }
}

BitBoard&
PieceSet::get_mut(Type type) noexcept
{
  switch (type) {
    case Type::King: return kings;
    case Type::Queen: return queens;
    case Type::Rook: return rooks;
    case Type::Bishop: return bishops;
    case Type::Knight: return knights;
    default: return pawns;
  }
}

// TODO: refactor this to take Sq parameter to avoid having to do an assert.
std::optional<Piece>
PieceSet::find_type(BitBoard bb) const noexcept
{
  assert(bb.has_single_bit()
        and "BitBoard bb should only have 1 bit set.");
  if (bb & kings) return Piece(Type::King);
  if (bb & queens) return Piece(Type::Queen);
  if (bb & rooks) return Piece(Type::Rook);
  if (bb & bishops) return Piece(Type::Bishop);
  if (bb & knights) return Piece(Type::Knight);
  if (bb & pawns) return Piece(Type::Pawn);
  return std::nullopt;
}

PieceSet
PieceSet::init_white() noexcept
{
  PieceSet pieces;
  pieces.kings = kWhiteKing,
  pieces.queens = kWhiteQueen;
  pieces.rooks = kWhiteRooks;
  pieces.bishops = kWhiteBishops;
  pieces.knights = kWhiteKnights;
  pieces.pawns = kWhitePawns;
  pieces.all_bits = kWhitePieces;
  pieces.color = Color::White;
  pieces.king_castle = true;
  pieces.queen_castle = false;
  return pieces;
}

PieceSet
PieceSet::init_black() noexcept
{
  PieceSet pieces;
  pieces.kings = kBlackKing,
  pieces.queens = kBlackQueen;
  pieces.rooks = kBlackRooks;
  pieces.bishops = kBlackBishops;
  pieces.knights = kBlackKnights;
  pieces.pawns = kBlackPawns;
  pieces.all_bits = kBlackPieces;
  pieces.color = Color::Black;
  pieces.king_castle = true;
  pieces.queen_castle = false;
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
     and all().count() <= 16;
}

PieceSet
PieceSet::flip() const noexcept
{
  auto flipped = *this;
  flipped.kings.flip_this();
  flipped.queens.flip_this();
  flipped.rooks.flip_this();
  flipped.bishops.flip_this();
  flipped.knights.flip_this();
  flipped.pawns.flip_this();
  flipped.all_bits.flip_this();
  return flipped;
}

} // namespace blunder
