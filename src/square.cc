#include "square.h"

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

} // namespace blunder
