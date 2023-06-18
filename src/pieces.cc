#include "pieces.h"

#include <bit>
#include <cassert>
#include <cctype>

namespace blunder {

std::uint8_t
GetPieceUint8(const std::array<BitBoard, 6> pieces, BitBoard piece) noexcept
{
  assert(std::has_single_bit(piece));

  for (std::uint8_t i = 0; i < pieces.size(); ++i) {
    if (piece & pieces[i])
      return i;
  }

  return static_cast<std::uint8_t>(Piece::None);
}

std::uint8_t
AsciiLetter(Piece piece, Color color) noexcept
{
  unsigned char letter = '?';

  switch (piece) {
    case Piece::King:
      letter = 'K';
      break;
    case Piece::Queen:
      letter = 'Q';
      break;
    case Piece::Rook:
      letter = 'R';
      break;
    case Piece::Bishop:
      letter = 'B';
      break;
    case Piece::Knight:
      letter = 'N';
      break;
    case Piece::Pawn:
      letter = 'P';
      break;
    default:
      break;
  }

  return color == Color::White
       ? letter
       : std::tolower(letter);
}

} // namespace
