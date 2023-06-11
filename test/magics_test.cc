#include "magics.h"

#include <memory>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "pre_computed_magics.h"
#include "square.h"
#include "utils.h"

using namespace blunder;

using ::testing::SizeIs;

constinit std::unique_ptr<MagicAttacks> bmagics = nullptr;
constinit std::unique_ptr<MagicAttacks> rmagics = nullptr;

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

TEST_F(BishopMagicsTest, WhenBlockedImmediatelyOnAllSides)
{
  auto square = ToUint(Sq::e4);
  auto blockers = ToBitBoard({Sq::d3, Sq::d5, Sq::f5, Sq::f3});
  EXPECT_THAT(
      bmagics->GetAttacks(square, blockers),
      EqualToSq(SqList{Sq::d3, Sq::f3, Sq::d5, Sq::f5}));
}

class RookMagicsTest : public testing::Test {
protected:
  void
  SetUp() override
  {
    if (not rmagics) {
      auto result = InitFromRookMagics(kRookMagics);
      ASSERT_TRUE(result) << "Unable to initialize bishop magics.";
      rmagics = std::make_unique<MagicAttacks>(std::move(*result));
    }
  }
};

TEST_F(RookMagicsTest, Contains64Magics)
{
  EXPECT_THAT(bmagics->GetMagics(), SizeIs(64));
}

TEST_F(RookMagicsTest, WhenBlockedImmediatelyOnAllSides)
{
  auto square = ToUint(Sq::e4);
  auto blockers = ToBitBoard({Sq::e3, Sq::d4, Sq::f4, Sq::e5});
  EXPECT_THAT(
      rmagics->GetAttacks(square, blockers),
      EqualToSq(SqList{Sq::e3, Sq::d4, Sq::f4, Sq::e5}));
}
