#include "bitboard.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "square.h"
#include "utils.h"

using namespace blunder;

TEST(GetRookAttacks, FromE4AllBlocked) {
  auto square = ToUint(Sq::e4);
  auto blockers = ToBitBoard(SqList{Sq::e3, Sq::d4, Sq::e5, Sq::f4});
  EXPECT_THAT(
      GetRookAttacks(square, blockers),
      EqualToSq(SqList{Sq::e3, Sq::d4, Sq::e5, Sq::f4}));
}

TEST(GetRookAttacks, FromE4BlockedTwoSquaresAway) {
  auto square = ToUint(Sq::e4);
  auto blockers = ToBitBoard(SqList{Sq::e2, Sq::c4, Sq::e6, Sq::g4});
  EXPECT_THAT(
      GetRookAttacks(square, blockers),
      EqualToSq(SqList{
        Sq::e2, Sq::e3, Sq::d4, Sq::c4, Sq::e5, Sq::e6, Sq::f4, Sq::g4}));
}

TEST(GetRookAttacks, FromB2WithTwoBlockers) {
  auto square = ToUint(Sq::b2);
  auto blockers = ToBitBoard(SqList{Sq::f2, Sq::b6});
  EXPECT_THAT(
      GetRookAttacks(square, blockers),
      EqualToSq(SqList{
        Sq::c2, Sq::d2, Sq::e2, Sq::f2, Sq::b3, Sq::b4, Sq::b5, Sq::b6}));
}

TEST(GetRookAttacks, FromH6WithThreeBlockers) {
  auto square = ToUint(Sq::h6);
  auto blockers = ToBitBoard(SqList{Sq::g6, Sq::h3});
  EXPECT_THAT(
      GetRookAttacks(square, blockers),
      EqualToSq(SqList{ Sq::g6, Sq::h3, Sq::h4, Sq::h5, Sq::h7}));
}

TEST(GetBishopAttacks, FromE4AllBlocked) {
  auto square = ToUint(Sq::e4);
  auto blockers = ToBitBoard(SqList{Sq::d3, Sq::f3, Sq::d5, Sq::f5});
  EXPECT_THAT(
      GetBishopAttacks(square, blockers),
      EqualToSq(SqList{Sq::d3, Sq::f3, Sq::d5, Sq::f5}));
}

TEST(GetBishopAttacks, FromE4BlockedTwoSquaresAway) {
  auto square = ToUint(Sq::e4);
  auto blockers = ToBitBoard(SqList{Sq::c2, Sq::g2, Sq::c6, Sq::g6});
  EXPECT_THAT(
      GetBishopAttacks(square, blockers),
      EqualToSq(SqList{Sq::c2, Sq::g2, Sq::d3, Sq::f3, Sq::d5, Sq::f5, Sq::c6,
        Sq::g6}));
}

TEST(GetBishopAttacks, FromB2WithOneBlocker) {
  auto square = ToUint(Sq::b2);
  auto blockers = ToBitBoard(SqList{Sq::f6});
  EXPECT_THAT(
      GetBishopAttacks(square, blockers),
      EqualToSq(SqList{Sq::c3, Sq::d4, Sq::e5, Sq::f6}));
}

TEST(GetBishopAttacks, FromH6WithNoBlockers) {
  auto square = ToUint(Sq::h6);
  auto blockers = ToBitBoard(SqList{});
  EXPECT_THAT(
      GetBishopAttacks(square, blockers),
      EqualToSq(SqList{Sq::d2, Sq::e3, Sq::f4, Sq::g5, Sq::g7}));
}
