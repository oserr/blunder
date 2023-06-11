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

TEST_F(BishopMagicsTest, FromE4)
{
  auto square = ToUint(Sq::e4);
  auto blockers = ToBitBoard({Sq::d3, Sq::d5, Sq::f5, Sq::f3});
  EXPECT_THAT(
      bmagics->GetAttacks(square, blockers),
      EqualToSq(SqList{Sq::d3, Sq::f3, Sq::d5, Sq::f5}));
}

TEST_F(BishopMagicsTest, FromB1)
{
  auto square = ToUint(Sq::b1);
  auto blockers = ToBitBoard({Sq::e4, Sq::f5});
  EXPECT_THAT(
      bmagics->GetAttacks(square, blockers),
      EqualToSq(SqList{Sq::c2, Sq::d3, Sq::e4}));
}

TEST_F(BishopMagicsTest, FromC4)
{
  auto square = ToUint(Sq::c4);
  auto blockers = ToBitBoard({Sq::b3, Sq::e6, Sq::f7, Sq::g8});
  EXPECT_THAT(
      bmagics->GetAttacks(square, blockers),
      EqualToSq(SqList{Sq::b3, Sq::d3, Sq::e2, Sq::b5, Sq::d5, Sq::e6}));
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

TEST_F(RookMagicsTest, FromE4)
{
  auto square = ToUint(Sq::e4);
  auto blockers = ToBitBoard({Sq::e3, Sq::d4, Sq::f4, Sq::e5});
  EXPECT_THAT(
      rmagics->GetAttacks(square, blockers),
      EqualToSq(SqList{Sq::e3, Sq::d4, Sq::f4, Sq::e5}));
}

TEST_F(RookMagicsTest, FromA1)
{
  auto square = ToUint(Sq::a1);
  auto blockers = ToBitBoard({Sq::a4, Sq::e3, Sq::f4, Sq::e1});
  EXPECT_THAT(
      rmagics->GetAttacks(square, blockers),
      EqualToSq(SqList{
        Sq::b1, Sq::c1, Sq::d1, Sq::e1, Sq::a2, Sq::a3, Sq::a4}));
}

TEST_F(RookMagicsTest, FromF5)
{
  auto square = ToUint(Sq::f5);
  auto blockers = ToBitBoard({Sq::f2, Sq::f3, Sq::d5});
  EXPECT_THAT(
      rmagics->GetAttacks(square, blockers),
      EqualToSq(SqList{
        Sq::f3, Sq::f4, Sq::f6, Sq::f7, Sq::e5, Sq::d5, Sq::g5}));
}
