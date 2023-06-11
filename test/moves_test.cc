#include "moves.h"

#include <cstdint>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "square.h"
#include "utils.h"

using namespace blunder;

TEST(MoveNorth, ShiftsFromA1ToA2)
{
  auto bb = ToBitBoard(Sq::a1);
  EXPECT_THAT(MoveNorth(bb), EqualToSq(SqList{Sq::a2}));
}

TEST(MoveNorth, ShiftsFromA8ToOutOfBoard)
{
  auto bb = ToBitBoard(Sq::a8);
  EXPECT_THAT(MoveNorth(bb), EqualToSq(SqList{}));
}

TEST(MoveSouth, ShiftsFromE4ToE3)
{
  auto bb = ToBitBoard(Sq::e4);
  EXPECT_THAT(MoveSouth(bb), EqualToSq(SqList{Sq::e3}));
}

TEST(MoveSouth, ShiftsFromE1ToOutOfBoard)
{
  auto bb = ToBitBoard(Sq::e1);
  EXPECT_THAT(MoveSouth(bb), EqualToSq(SqList{}));
}

TEST(MoveKing, FromA1)
{
  auto king = ToBitBoard(Sq::a1);
  EXPECT_THAT(MoveKing(king), EqualToSq(SqList{Sq::b1, Sq::a2, Sq::b2}));
}

TEST(MoveKing, FromE1)
{
  auto king = ToBitBoard(Sq::e1);
  auto moves = SqList{Sq::d1, Sq::d2, Sq::e2, Sq::f1, Sq::f2};
  EXPECT_THAT(MoveKing(king), EqualToSq(moves));
}

TEST(MoveKing, FromH5)
{
  auto king = ToBitBoard(Sq::h5);
  auto moves = SqList{Sq::h4, Sq::g4, Sq::g5, Sq::g6, Sq::h6};
  EXPECT_THAT(MoveKing(king), EqualToSq(moves));
}

TEST(MoveKing, FromE5)
{
  auto king = ToBitBoard(Sq::e5);
  auto moves = SqList{
    Sq::d4, Sq::d5, Sq::d6, Sq::e4, Sq::e6, Sq::f4, Sq::f5, Sq::f6};
  EXPECT_THAT(MoveKing(king), EqualToSq(moves));
}

TEST(MoveKing, FromG8)
{
  auto king = ToBitBoard(Sq::g8);
  auto moves = SqList{Sq::f8, Sq::h8, Sq::f7, Sq::g7, Sq::h7};
  EXPECT_THAT(MoveKing(king), EqualToSq(moves));
}

TEST(MoveKnight, FromB1)
{
  auto knight = ToBitBoard(Sq::b1);
  auto moves = SqList{Sq::a3, Sq::c3, Sq::d2};
  EXPECT_THAT(MoveKnight(knight), EqualToSq(moves));
}

TEST(MoveKnight, FromD4)
{
  auto knight = ToBitBoard(Sq::d4);
  auto moves = SqList{
    Sq::c2, Sq::b3, Sq::b5, Sq::c6, Sq::e6, Sq::f5, Sq::f3, Sq::e2};
  EXPECT_THAT(MoveKnight(knight), EqualToSq(moves));
}

TEST(MoveKnight, FromG5)
{
  auto knight = ToBitBoard(Sq::g5);
  auto moves = SqList{Sq::f3, Sq::e4, Sq::e6, Sq::f7, Sq::h7, Sq::h3};
  EXPECT_THAT(MoveKnight(knight), EqualToSq(moves));
}

TEST(MoveKnight, FromA8)
{
  auto knight = ToBitBoard(Sq::a8);
  auto moves = SqList{Sq::b6, Sq::c7};
  EXPECT_THAT(MoveKnight(knight), EqualToSq(moves));
}

TEST(MoveWhitePawnsSingle, OneSquareForward)
{
  auto pawns = ToBitBoard({Sq::a2, Sq::c6, Sq::d5, Sq::g7, Sq::h8});
  auto moves = SqList{Sq::a3, Sq::c7, Sq::d6, Sq::g8};
  EXPECT_THAT(MoveWhitePawnsSingle(pawns, ~0ull), EqualToSq(moves));
}

TEST(MoveWhitePawnsDouble, TwoSquaresForward)
{
  auto pawns = ToBitBoard({Sq::a2, Sq::c6, Sq::d5, Sq::g2, Sq::h2});
  auto moves = SqList{Sq::a4, Sq::g4, Sq::h4};
  EXPECT_THAT(MoveWhitePawnsDouble(pawns, ~0ull), EqualToSq(moves));
}

TEST(MoveWhitePawnsAttackLeft, AttackLeft)
{
  auto pawns = ToBitBoard({Sq::a2, Sq::c6, Sq::g2, Sq::h4});
  auto moves = SqList{Sq::b7, Sq::f3, Sq::g5};
  EXPECT_THAT(MoveWhitePawnsAttackLeft(pawns, ~0ull), EqualToSq(moves));
}

TEST(MoveWhitePawnsAttackRight, AttackRight)
{
  auto pawns = ToBitBoard({Sq::a2, Sq::c6, Sq::g2, Sq::h4});
  auto moves = SqList{Sq::b3, Sq::d7, Sq::h3};
  EXPECT_THAT(MoveWhitePawnsAttackRight(pawns, ~0ull), EqualToSq(moves));
}

TEST(MoveBlackPawnsSingle, OneSquareForward)
{
  auto pawns = ToBitBoard({Sq::b7, Sq::c6, Sq::d7, Sq::g8, Sq::h3});
  auto moves = SqList{Sq::b6, Sq::c5, Sq::d6, Sq::g7, Sq::h2};
  EXPECT_THAT(MoveBlackPawnsSingle(pawns, ~0ull), EqualToSq(moves));
}

TEST(MoveBlackPawnsDouble, TwoSquaresForward)
{
  auto pawns = ToBitBoard({Sq::b7, Sq::c6, Sq::d7, Sq::g8, Sq::h3});
  auto moves = SqList{Sq::b5, Sq::d5};
  EXPECT_THAT(MoveBlackPawnsDouble(pawns, ~0ull), EqualToSq(moves));
}

TEST(MoveBlackPawnsAttackLeft, AttackLeft)
{
  auto pawns = ToBitBoard({Sq::b7, Sq::c6, Sq::d7, Sq::h3});
  auto moves = SqList{Sq::c6, Sq::d5, Sq::e6};
  EXPECT_THAT(MoveBlackPawnsAttackLeft(pawns, ~0ull), EqualToSq(moves));
}

TEST(MoveBlackPawnsAttackRight, AttackRight)
{
  auto pawns = ToBitBoard({Sq::a7, Sq::c6, Sq::d7, Sq::h3});
  auto moves = SqList{Sq::b5, Sq::c6, Sq::g2};
  EXPECT_THAT(MoveBlackPawnsAttackRight(pawns, ~0ull), EqualToSq(moves));
}
