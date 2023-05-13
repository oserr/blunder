#include "bitboard_io.h"

#include <algorithm>
#include <array>
#include <bit>
#include <cstddef>
#include <string>

#include "bitboard.h"

namespace blunder {

std::string
ToMailboxStr(BitBoard board)
{
  // Reverse the bytes to start from rank 8.
  auto other_val = std::bit_cast<std::array<std::byte, sizeof(board)>>(board);
  std::ranges::reverse(other_val);
  board = std::bit_cast<BitBoard>(other_val);

  std::string buff;
  buff.reserve(1024);

  for (size_t i = 0; i < 8; ++i, board >>= 8) {
    buff +=  "+---+---+---+---+---+---+---+---+\n|";
    BitBoard bits = board & 0xff;
    for (size_t j = 0; j < 8; ++j, bits >>= 1)
      buff += bits & 1 ? " 1 |" : "   |";
    buff += '\n';
  }

  buff +=  "+---+---+---+---+---+---+---+---+\n";

  return buff;
}

} // namespace blunder
