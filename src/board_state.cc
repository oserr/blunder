#include "board_state.h"

#include <algorithm>
#include <bit>
#include <format>
#include <iterator>

#include "bitboard.h"
#include "pieces.h"
#include "square.h"

namespace blunder {
namespace {

namespace rng = std::ranges;

void
FillAsciiBoard(
    const PieceSet& pieces,
    Color color,
    std::array<char, 64>& board)  noexcept
{
  const Piece piece_type[] = {
    Piece::King,
    Piece::Queen,
    Piece::Rook,
    Piece::Bishop,
    Piece::Knight,
    Piece::Pawn
  };

  for (auto type : piece_type) {
    auto bb = pieces[Uint8(type)];
    auto squares = ToSetOfSq(bb);
    for (auto sq : squares) {
      auto index = ToUint(sq);
      board[index] = AsciiLetter(type, color);
    }
  }
}

} // namespace

BitBoard
AllMask(const PieceSet& pieces) noexcept
{
  BitBoard mask = 0ull;
  for (auto piece : pieces)
    mask |= piece;
  return mask;
}

BoardState
NewBoardState() noexcept
{
  BoardState state;

  state.mine[Uint8(Piece::King)] = kWhiteKing;
  state.mine[Uint8(Piece::Queen)] = kWhiteQueen;
  state.mine[Uint8(Piece::Rook)] = kWhiteRooks;
  state.mine[Uint8(Piece::Bishop)] = kWhiteBishops;
  state.mine[Uint8(Piece::Knight)] = kWhiteKnights;
  state.mine[Uint8(Piece::Pawn)] = kWhitePawns;

  state.other[Uint8(Piece::King)] = kBlackKing;
  state.other[Uint8(Piece::Queen)] = kBlackQueen;
  state.other[Uint8(Piece::Rook)] = kBlackRooks;
  state.other[Uint8(Piece::Bishop)] = kBlackBishops;
  state.other[Uint8(Piece::Knight)] = kBlackKnights;
  state.other[Uint8(Piece::Pawn)] = kBlackPawns;

  state.all_mine = kWhitePieces;
  state.all_other = kBlackPieces;

  state.half_move = 1;
  state.full_move = 0;

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
operator==(const BoardState& left, const BoardState& right) noexcept
{
  return left.mine == right.mine
     and left.other == right.other
     and left.all_mine == right.all_mine
     and left.all_other == right.all_other
     and left.half_move == right.half_move
     and left.full_move == right.full_move
     and left.en_passant == right.en_passant
     and left.en_passant_file == right.en_passant_file
     and left.wk_castle == right.wk_castle
     and left.wq_castle == right.wq_castle
     and left.bk_castle == right.bk_castle
     and left.bq_castle == right.bq_castle;
}

std::string
DebugStr(const BoardState& state)
{
  std::array<char, 64> board;
  board.fill('-');
  FillAsciiBoard(state.White(), Color::White, board);
  FillAsciiBoard(state.Black(), Color::Black, board);

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
  buff += state.next == Color::White ? 'w' : 'b';
  buff += '\n';

  buff += "Castling:";
  if (state.wk_castle)
    buff += 'K';
  if (state.wq_castle)
    buff += 'Q';
  if (state.bk_castle)
    buff += 'k';
  if (state.bq_castle)
    buff += 'q';

  buff += '\n';

  std::format_to(std::back_inserter(buff), "HalfMove: {}\n", state.half_move);
  std::format_to(std::back_inserter(buff), "FullMove: {}\n", state.full_move);

  if (state.en_passant)
    std::format_to(
        std::back_inserter(buff), "EnPassant: {}\n", state.en_passant_file);

  return buff;
}

} // namespace blunder
