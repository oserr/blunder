#include "square.h"

#include <bit>
#include <cassert>

namespace blunder {

std::string
ToSqStr(unsigned val)
{
  assert(val < 64);
  auto row = val / 8;
  auto col = val % 8;

  char letters[3];
  letters[0] = 'a' + col;
  letters[1] = '1' + row;
  letters[2] = 0;

  return letters;
}

std::set<Sq>
ToSetOfSq(BitBoard bb)
{
  std::set<Sq> squares;
  for (; bb; bb &= bb - 1) {
    unsigned index = std::countr_zero(bb);
    squares.insert(ToSq(index));
  }
  return squares;
}

BitBoard
ToBitBoard(std::initializer_list<Sq> squares) noexcept
{
  BitBoard bb = 0;
  for (auto sq : squares)
    bb |= ToBitBoard(sq);
  return bb;
}
} // namespace blunder
