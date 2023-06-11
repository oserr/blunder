#pragma once

#include "gtest/gtest.h"
#include "square.h"

namespace blunder {

MATCHER_P(EqualToSq, squares, "has the following squares set: "
    + ToListStr(squares))
{
  auto bb = ToBitBoard(squares);
  if (bb == arg) return true;

  auto sq_set = ToSetOfSq(arg);
  *result_listener << ToListStr(sq_set);

  return false;
}

} // namespace blunder
