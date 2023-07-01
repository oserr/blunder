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
  while (bb)
    squares.insert(ToSq(bb.first_bit_and_clear()));
  return squares;
}

BitBoard
ToBitBoard(SqList squares) noexcept
{
  BitBoard bb;
  for (auto sq : squares)
    bb |= ToBitBoard(sq);
  return bb;
}

std::string
ToListStr(const std::set<Sq>& squares)
{
  std::string out;
  out.reserve(squares.size() * 6);

  out += '[';

  auto first = squares.begin();
  auto last = squares.end();

  if (first != last)
    out += ToStr(*first++);

  while (first != last)
    out += ", " + ToStr(*first++);

  out += ']';

  return out;
}

std::string
ToListStr(SqList squares)
{
  std::set<Sq> sqs{squares.begin(), squares.end()};
  return ToListStr(sqs);
}

} // namespace blunder
