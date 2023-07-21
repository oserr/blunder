#include "board.h"

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

//---------------------------
// Move generation utilities.
//---------------------------

// Returns all the non-attack moves for |piece| from square |from_square| to all
// squares in |to_squares|. The moves are returned in |moves|, an output
// parameter.
inline void
get_non_attacks(
    Piece piece,
    std::uint8_t from_square,
    BitBoard to_squares,
    MoveVec& moves)
{
  while (to_squares)
    moves.emplace_back(piece, from_square, to_squares.first_bit_and_clear());
}

// Returns all the simple attack moves for |piece| from square |from_square| to
// squares in |to_squares|. We pass in the |other| pieces to be able to figure
// out what the attacked pieces are.
void
get_simple_attacks(
    Piece piece,
    std::uint8_t from_square,
    BitBoard to_squares,
    const PieceSet& other,
    MoveVec& moves)
{
  while (to_squares) {
    auto [to_square, attacked] = to_squares.index_bb_and_clear();
    auto to_piece = other.find_type(attacked);
    assert(to_piece.type() != Type::None);
    moves.emplace_back(piece, from_square, to_piece, to_square);
  }
}

} // namespace

Board
Board::new_board() noexcept
{
  Board state;

  state.bb_mine = PieceSet::init_white();
  state.bb_other = PieceSet::init_black();

  state.half_move = 0;
  state.full_move = 1;

  state.next = Color::White;

  state.en_passant = false;
  state.en_passant_file = false;

  state.wk_castle = true;
  state.wq_castle = true;
  state.bk_castle = true;
  state.bq_castle = true;

  return state;
}

bool
Board::eq(const Board& bs) const noexcept
{
  return bb_mine == bs.bb_mine
     and bb_other == bs.bb_other
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
Board::str() const
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

//-----------------
// Move generation.
//-----------------

// TODO: check any moves are illegal. For example:
// - king cannot put himself in check.
// - castling is not possible if one of the squares the king has to cross is
//   attacked.
//
// This can be done while we generating the king moves, to prevent creating a
// move that is not legal, or after the moves are generated. The latter might
// be necessary and simpler to do.
MoveVec
Board::king_moves() const
{
  MoveVec moves;
  get_simple_moves(Piece::pawn(), move_king, moves);

  if (is_white_next()) {
    if (wk_can_castle())
      moves.push_back(Move::wk_castle());
    if (wq_can_castle())
      moves.push_back(Move::wq_castle());
  } else {
    if (bk_can_castle())
      moves.push_back(Move::bk_castle());
    if (bq_can_castle())
      moves.push_back(Move::wq_castle());
  }

  return moves;
}

MoveVec
Board::bishop_moves() const
{
  assert(bmagics);
  auto moves_fn = [&](BitBoard bb) {
    auto from_square = bb.first_bit();
    return bmagics->get_attacks(from_square, all_bits());
  };
  return get_simple_moves(Piece::bishop(), moves_fn);
}

void
Board::get_simple_moves(
    Piece piece,
    const std::function<BitBoard(BitBoard)>& moves_fn,
    MoveVec& moves) const
{
  auto bb = mine().get(piece);
  auto no_pieces = none();

  while (bb) {
    auto [from_square, bb_piece] = bb.index_bb_and_clear();
    auto bb_moves = moves_fn(bb_piece);

    // Compute moves to empty squares.
    auto to_squares = bb_moves & no_pieces;
    get_non_attacks(piece, from_square, to_squares, moves);

    // Compute attacks.
    to_squares = bb_moves & all_other();
    get_simple_attacks(piece, from_square, to_squares, other(), moves);
  }
}

} // namespace blunder
