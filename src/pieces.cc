#include "pieces.h"

#include <bit>
#include <cassert>

namespace blunder {

std:uint8_t
GetPieceUint8(const std::array<BitBoard, 6> pieces, BitBoard piece) noexcept
{
  assert(std::has_single_bit(piece));

  for (std::uint8_t i = 0; i < pieces.size(); ++i) {
    if (piece & pieces[i])
      return i;
  }

  return static_cast<std:uint8_t>(Pice::None);
}

} // namespace
