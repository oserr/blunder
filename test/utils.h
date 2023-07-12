#pragma once

#include "gtest/gtest.h"
#include "square.h"

namespace blunder {

MATCHER_P(EqualToSq, squares, "has the following squares set: "
    + to_list_str(squares))
{
  auto bb = to_bitboard(squares);
  if (bb == arg) return true;

  auto sq_set = to_set_of_sq(arg);
  *result_listener << to_list_str(sq_set);

  return false;
}

} // namespace blunder
