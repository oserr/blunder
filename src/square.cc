#include "square.h"

#include <bit>
#include <cassert>

namespace blunder {

std::string
to_sq_str(unsigned val)
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
to_set_of_sq(BitBoard bb)
{
  std::set<Sq> squares;
  while (bb)
    squares.insert(to_sq(bb.first_bit_and_clear()));
  return squares;
}

BitBoard
to_bitboard(SqList squares) noexcept
{
  BitBoard bb;
  for (auto sq : squares)
    bb |= to_bitboard(sq);
  return bb;
}

std::string
to_list_str(const std::set<Sq>& squares)
{
  std::string out;
  out.reserve(squares.size() * 6);

  out += '[';

  auto first = squares.begin();
  auto last = squares.end();

  if (first != last)
    out += str(*first++);

  while (first != last)
    out += ", " + str(*first++);

  out += ']';

  return out;
}

std::string
to_list_str(SqList squares)
{
  std::set<Sq> sqs{squares.begin(), squares.end()};
  return to_list_str(sqs);
}

} // namespace blunder
