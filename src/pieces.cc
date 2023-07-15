#include "pieces.h"

#include <cassert>
#include <cctype>

namespace blunder {

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
