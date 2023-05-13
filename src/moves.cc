#include "moves.h"

#include "bitboard.h"

namespace blunder {
namespace {

inline constexpr BitBoard
MoveNorth(BitBoard bb) noexcept { return bb << 8; }

inline constexpr BitBoard
MoveSouth(BitBoard bb) noexcept { return bb >> 8; }
} // namespace

constexpr BitBoard
MoveKing(BitBoard king) noexcept
{
  const auto left = (king & ~kFileA) >> 1; 
  const auto right = (king & ~kFileH) << 1; 
  king |= left | right;
  const auto down = (king & ~kRank1) >> 8;
  const auto up = (king & ~kRank8) << 8;
  return down | up;
}

constexpr BitBoard
MoveQueen(BitBoard queen) noexcept
{
  return queen;
}

constexpr BitBoard
MoveRook(BitBoard rook) noexcept
{
  return rook;
}

constexpr BitBoard
MoveBishop(BitBoard bishop) noexcept
{
  return bishop;
}

constexpr BitBoard
MoveKnight(BitBoard knight) noexcept
{
  // One square up, two right.
  const auto bits1 = (knight & ~kFileG & ~kFileH) << 10;
  // Two square up, one right.
  const auto bits2 = (knight & ~kFileH) << 17;
  // Two square up, one left.
  const auto bits3 = (knight & ~kFileA) << 15;
  // One square up, two left.
  const auto bits4 = (knight & ~kFileA & ~kFileB) << 6;
  // One square down, two left.
  const auto bits5 = (knight & ~kFileA & ~kFileB) >> 10;
  // Two square down, one left.
  const auto bits6 = (knight & ~kFileA) >> 17;
  // Two square down, one right.
  const auto bits7 = (knight & ~kFileH) >> 15;
  // One square down, two right.
  const auto bits8 = (knight & ~kFileG & ~kFileH) >> 6;

  return bits1 | bits2 | bits3 | bits4
       | bits5 | bits6 | bits7 | bits8;
}

constexpr BitBoard
MoveWhitePawnsSingle(BitBoard pawns, BitBoard empty) noexcept
{
  return MoveNorth(pawns) & empty;
}

constexpr BitBoard
MoveWhitePawnsDouble(BitBoard pawns, BitBoard empty) noexcept
{
  pawns = MovePawnsWhiteSingle(pawns, empty);
  return MovePawnsWhiteSingle(pawns, empty) & kRank4;
}

constexpr BitBoard
MoveWhitePawnsAttackLeft(BitBoard pawns, BitBoard all_black) noexcept
{
  return pawns << 7 & all_black & ~kFileH.
}

constexpr BitBoard
MoveWhitePawnsAttackRight(BitBoard pawns, BitBoard all_black) noexcept
{
  return pawns << 9 & all_black & ~kFileA.
}

constexpr BitBoard
MoveBlackPawnsSingle(BitBoard pawns, BitBoard empty) noexcept
{
  return MoveSouth(pawns) & empty;
}

constexpr BitBoard
MoveBlackPawnsDouble(BitBoard pawns, BitBoard empty) noexcept
{
  pawns = MovePawnsBlackSingle(pawns, empty);
  return MovePawnsBlackSingle(pawns, empty) & kRank5;
}

constexpr BitBoard
MoveBlackPawnsAttackLeft(BitBoard pawns, BitBoard all_white) noexcept
{
  return pawns >> 7 & all_white & ~kFileA.
}

constexpr BitBoard
MoveBlackPawnsAttackRight(BitBoard pawns, BitBoard all_white) noexcept
{
  return pawns >> 9 & all_white & ~kFileH.
}

} // namespace blunder
