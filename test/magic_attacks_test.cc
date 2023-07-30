#include "magic_attacks.h"

#include <memory>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "pre_computed_magics.h"
#include "square.h"
#include "utils.h"

using namespace blunder;

using ::testing::ContainerEq;
using ::testing::SizeIs;

constinit std::unique_ptr<MagicAttacks> bmagics = nullptr;
constinit std::unique_ptr<MagicAttacks> rmagics = nullptr;

class BishopMagicsTest : public testing::Test {
protected:
  void
  SetUp() override
  {
    if (not bmagics) {
      auto result = from_bmagics(kBishopMagics);
      ASSERT_TRUE(result) << "Unable to initialize bishop magics.";
      bmagics = std::make_unique<MagicAttacks>(std::move(*result));
    }
  }
};

TEST_F(BishopMagicsTest, Contains64Magics)
{
  EXPECT_THAT(bmagics->get_magics(), SizeIs(64));
}

TEST_F(BishopMagicsTest, FromE4)
{
  auto square = to_int(Sq::e4);
  auto blockers = to_bitboard({Sq::d3, Sq::d5, Sq::f5, Sq::f3});
  EXPECT_THAT(
      bmagics->get_attacks(square, blockers),
      EqualToSq(SqList{Sq::d3, Sq::f3, Sq::d5, Sq::f5}));
}

TEST_F(BishopMagicsTest, FromB1)
{
  auto square = to_int(Sq::b1);
  auto blockers = to_bitboard({Sq::e4, Sq::f5});
  EXPECT_THAT(
      bmagics->get_attacks(square, blockers),
      EqualToSq(SqList{Sq::a2, Sq::c2, Sq::d3, Sq::e4}));
}

TEST_F(BishopMagicsTest, FromC4)
{
  auto square = to_int(Sq::c4);
  auto blockers = to_bitboard({Sq::b3, Sq::e6, Sq::f7, Sq::g8});
  EXPECT_THAT(
      bmagics->get_attacks(square, blockers),
      EqualToSq(SqList{Sq::f1, Sq::b3, Sq::d3, Sq::e2,
                       Sq::b5, Sq::d5, Sq::a6, Sq::e6}));
}

class RookMagicsTest : public testing::Test {
protected:
  void
  SetUp() override
  {
    if (not rmagics) {
      auto result = from_rmagics(kRookMagics);
      ASSERT_TRUE(result) << "Unable to initialize bishop magics.";
      rmagics = std::make_unique<MagicAttacks>(std::move(*result));
    }
  }
};

TEST_F(RookMagicsTest, Contains64Magics)
{
  EXPECT_THAT(bmagics->get_magics(), SizeIs(64));
}

TEST_F(RookMagicsTest, FromE4)
{
  auto square = to_int(Sq::e4);
  auto blockers = to_bitboard({Sq::e3, Sq::d4, Sq::f4, Sq::e5});
  EXPECT_THAT(
      rmagics->get_attacks(square, blockers),
      EqualToSq(SqList{Sq::e3, Sq::d4, Sq::f4, Sq::e5}));
}

TEST_F(RookMagicsTest, FromA1)
{
  auto square = to_int(Sq::a1);
  auto blockers = to_bitboard({Sq::a4, Sq::e3, Sq::f4, Sq::e1});
  EXPECT_THAT(
      rmagics->get_attacks(square, blockers),
      EqualToSq(SqList{
        Sq::b1, Sq::c1, Sq::d1, Sq::e1, Sq::a2, Sq::a3, Sq::a4}));
}

TEST_F(RookMagicsTest, FromF5)
{
  auto square = to_int(Sq::f5);
  auto blockers = to_bitboard({Sq::f2, Sq::f3, Sq::d5});
  EXPECT_THAT(
      rmagics->get_attacks(square, blockers),
      EqualToSq(SqList{
        Sq::f3, Sq::f4, Sq::f6, Sq::f7,
        Sq::f8, Sq::e5, Sq::d5, Sq::g5,
        Sq::h5}));
}

TEST(PermuteMask, ForThreeConsecutiveBits) {
  // Mask 111
  // Combos: 000, 001, 011, 111, 110, 100, 101, 010
  std::set<BitBoard> combos = {
    BitBoard(), BitBoard(0b001),
    BitBoard(0b011), BitBoard(0b111),
    BitBoard(0b110), BitBoard(0b100),
    BitBoard(0b101), BitBoard(0b010)};

  BitBoard mask(0b111);
  auto num_bits = mask.count();
  auto ncombos = 1u << num_bits;

  std::set<BitBoard> results;
  for (unsigned i = 0; i < ncombos; ++i)
    results.insert(permute_mask(i, num_bits, mask));

  EXPECT_THAT(results, ContainerEq(combos));
}

TEST(PermuteMask, ForThreeSeparateBits) {
  // Combos for mask: 0101010
  std::set<BitBoard> combos = {
    BitBoard(), BitBoard(0b0000010),
    BitBoard(0b0001010), BitBoard(0b0101010),
    BitBoard(0b0101000), BitBoard(0b0100000),
    BitBoard(0b0100010), BitBoard(0b0001000)};

  BitBoard mask(0b0101010);
  auto num_bits = mask.count();
  auto ncombos = 1u << num_bits;

  std::set<BitBoard> results;
  for (unsigned i = 0; i < ncombos; ++i)
    results.insert(permute_mask(i, num_bits, mask));

  EXPECT_THAT(results, ContainerEq(combos));
}
