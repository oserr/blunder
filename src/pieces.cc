#include "pieces.h"

#include <cassert>
#include <cctype>

namespace blunder {

std::uint8_t
GetPieceUint8(const std::array<BitBoard, 6> pieces, BitBoard piece) noexcept
{
  assert(piece.has_single_bit());

  for (std::uint8_t i = 0; i < pieces.size(); ++i) {
    if (piece & pieces[i])
      return i;
  }

  return static_cast<std::uint8_t>(Type::None);
}

std::uint8_t
Piece::letter(Color color) noexcept
{
  unsigned char letter = '?';

  switch (ptype) {
    case Type::King:
      letter = 'K';
      break;
    case Type::Queen:
      letter = 'Q';
      break;
    case Type::Rook:
      letter = 'R';
      break;
    case Type::Bishop:
      letter = 'B';
      break;
    case Type::Knight:
      letter = 'N';
      break;
    case Type::Pawn:
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
