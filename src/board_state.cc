#include "board_state.h"

#include <algorithm>
#include <bit>
#include <format>
#include <iterator>

#include "bitboard.h"
#include "pieces.h"
#include "piece_set.h"
#include "square.h"

namespace blunder {
namespace {

namespace rng = std::ranges;

void
fill_ascii_board(
    const PieceSet& pieces,
    Color color,
    std::array<char, 64>& board)  noexcept
{
  const Type piece_type[] = {
    Type::King,
    Type::Queen,
    Type::Rook,
    Type::Bishop,
    Type::Knight,
    Type::Pawn
  };

  for (auto type : piece_type) {
    auto bb = pieces.get(type);
    auto squares = to_set_of_sq(bb);
    for (auto sq : squares) {
      auto index = to_int(sq);
      board[index] = letter(type, color);
    }
  }
}

} // namespace

BitBoard
AllMask(const PieceSet& pieces) noexcept
{
  BitBoard mask;
  for (auto piece : pieces)
    mask |= piece;
  return mask;
}

BoardState
NewBoardState() noexcept
{
  BoardState state;

  state.mine = PieceSet::init_white();
  state.all_mine = state.mine.full_set();
  state.other = PieceSet::init_black();
  state.all_other = state.other.full_set();

  state.half_move = 0;
  state.full_move = 1;

  state.next = Color::White;

  state.en_passant = 0;
  state.en_passant_file = 0;

  state.wk_castle = 1;
  state.wq_castle = 1;
  state.bk_castle = 1;
  state.bq_castle = 1;

  return state;
}

bool
BoardState::eq(const BoardState& bs) const noexcept
{
  return mine == bs.mine
     and other == bs.other
     and all_mine == bs.all_mine
     and all_other == bs.all_other
     and half_move == bs.half_move
     and full_move == bs.full_move
     and en_passant == bs.en_passant
     and en_passant_file == bs.en_passant_file
     and wk_castle == bs.wk_castle
     and wq_castle == bs.wq_castle
     and bk_castle == bs.bk_castle
     and bq_castle == bs.bq_castle;
}

std::string
BoardState::str() const
{
  std::array<char, 64> board;
  board.fill('-');
  fill_ascii_board(white(), Color::White, board);
  fill_ascii_board(black(), Color::Black, board);

  auto rev_board = std::bit_cast<std::array<std::uint64_t, 8>>(board);
  rng::reverse(rev_board);
  board = std::bit_cast<std::array<char, 64>>(rev_board);

  std::string buff;
  buff.reserve(256);

  auto first = board.data();
  auto last = first + board.size();

  buff += '\n';

  for (auto ptr = board.data(); ptr < last; ptr += 8) {
      buff += std::string_view(ptr, 8);
      buff += '\n';
  }

  buff += "Color:";
  buff += next == Color::White ? 'w' : 'b';
  buff += '\n';

  buff += "Castling:";
  if (wk_castle)
    buff += 'K';
  if (wq_castle)
    buff += 'Q';
  if (bk_castle)
    buff += 'k';
  if (bq_castle)
    buff += 'q';

  buff += '\n';

  std::format_to(std::back_inserter(buff), "HalfMove: {}\n", half_move);
  std::format_to(std::back_inserter(buff), "FullMove: {}\n", full_move);

  if (en_passant)
    std::format_to(
        std::back_inserter(buff), "EnPassant: {}\n", en_passant_file);

  return buff;
}

} // namespace blunder
