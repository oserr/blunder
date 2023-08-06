#include "board_path.h"

#include <memory>
#include <mutex>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "pre_computed_magics.h"
#include "magic_attacks.h"

using namespace blunder;

// Only init magic bitboards once because it's expensive to init the magics.
std::once_flag init_flag;

class BoardPathTest : public testing::Test
{
protected:
  void
  SetUp() override
  {
    std::call_once(init_flag, []{
      auto bmagics = from_bmagics(kBishopMagics);
      auto rmagics = from_rmagics(kRookMagics);
      ASSERT_TRUE(bmagics) << "Unable to init bishop magics for MoveGen.";
      ASSERT_TRUE(rmagics) << "Unable to init rook magics for MoveGen.";
      Board::register_magics(
          std::make_unique<MagicAttacks>(std::move(*bmagics)),
          std::make_unique<MagicAttacks>(std::move(*rmagics)));
    });
  }

  BoardPath board_path;
};

TEST_F(BoardPathTest, SizeIsZero)
{
  EXPECT_EQ(board_path.size(), 0);
}

TEST_F(BoardPathTest, NumIteratorsEqualToNumboards)
{
  int i = 0;
  for ([[maybe_unused]] const auto& board : board_path)
    ++i;

  EXPECT_EQ(i, 0);
  EXPECT_EQ(board_path.size(), 0);
  EXPECT_FALSE(board_path.is_full());

  Board board = Board::new_board();
  board_path.push(board);

  i = 0;
  for ([[maybe_unused]] const auto& board : board_path)
    ++i;

  EXPECT_EQ(i, 1);
  EXPECT_EQ(board_path.size(), 1);
  EXPECT_FALSE(board_path.is_full());

  board_path.push(board);

  i = 0;
  for ([[maybe_unused]] const auto& board : board_path)
    ++i;

  EXPECT_EQ(i, 2);
  EXPECT_EQ(board_path.size(), 2);
  EXPECT_FALSE(board_path.is_full());
}

TEST_F(BoardPathTest, OnlyAddsEightBoards)
{
  Board board = Board::new_board();

  for (int i = 0; i < 8; ++i) {
    EXPECT_FALSE(board_path.is_full());
    board_path.push(board);
  }

  EXPECT_TRUE(board_path.is_full());
  EXPECT_EQ(board_path.size(), 8);

  board_path.push(board);
  board_path.push(board);

  EXPECT_EQ(board_path.size(), 8);

  int i = 0;
  for ([[maybe_unused]] const auto& board : board_path)
    ++i;

  EXPECT_EQ(i, 8);
}

TEST_F(BoardPathTest, IteratesThroughCorrectBoards)
{
  std::vector<Board> boards;
  boards.reserve(8);
  for (int i = 0; i < 8; ++i) {
    const auto& board = boards.emplace_back(Board::new_board());
    board_path.push(board);
  }

  auto iter = boards.begin();
  auto last = boards.end();

  for (const auto& board : board_path) {
    ASSERT_NE(iter, last);
    EXPECT_EQ(&board, &*iter++);
  }
}
