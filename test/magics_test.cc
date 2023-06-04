#include "magics.h"

#include <memory>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "pre_computed_magics.h"
#include "square.h"

using namespace blunder;

using ::testing::SizeIs;

// TOOD: put this in utilities for testing.
MATCHER_P(EqualToSq, squares, "has the following squares set: "
    + ToListStr(squares))
{
  auto bb = ToBitBoard(squares);
  if (bb == arg) return true;

  auto sq_set = ToSetOfSq(arg);
  *result_listener << ToListStr(sq_set);

  return false;
}

constinit std::unique_ptr<MagicAttacks> bmagics = nullptr;

class BishopMagicsTest : public testing::Test {
protected:
  void
  SetUp() override
  {
    if (not bmagics) {
      auto result = InitFromBishopMagics(kBishopMagics);
      ASSERT_TRUE(result) << "Unable to initialize bishop magics.";
      bmagics = std::make_unique<MagicAttacks>(std::move(*result));
    }
  }
};

TEST_F(BishopMagicsTest, Contains64Magics)
{
  EXPECT_THAT(bmagics->GetMagics(), SizeIs(64));
}

TEST_F(BishopMagicsTest, NoMovesWhenBlocked)
{
  auto bishop = ToBitBoard({Sq::e4});
  auto blockers = ToBitBoard({Sq::d3, Sq::d5, Sq::f5, Sq::f3});
  EXPECT_THAT(bmagics->GetAttacks(bishop, blockers), EqualToSq(SqList{}));
}
