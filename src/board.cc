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
  for (auto to_square : to_squares.square_iter())
    moves.emplace_back(piece, from_square, to_square);
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

//----------------------------------------
// Helper functions to compute pawn moves.
//----------------------------------------

using PawnMovesFn = BitBoard(*)(BitBoard, BitBoard);
using FromFn = std::uint8_t(*)(std::uint8_t);
using IsPromoFn = bool(*)(std::uint8_t);

// Used to check if the move represents a promotion for white.
bool
is_white_promo(std::uint8_t to_square) noexcept
{ return to_square >= 56; }

// Used to check if the move represents a promotion for black.
bool
is_black_promo(std::uint8_t to_square) noexcept
{ return to_square <= 7; }

// Computes the from square for a one-square white pawn move given the
// destination square.
std::uint8_t
from_single_white(std::uint8_t to_square) noexcept
{ return to_square - 8; }

// Computes the from square for a double-square white pawn move given the
// destination square.
std::uint8_t
from_double_white(std::uint8_t to_square) noexcept
{ return to_square - 16; }

// Computes the from square for a white pawn attack diagonal left.
std::uint8_t
from_left_white(std::uint8_t to_square) noexcept
{ return to_square - 7; }

// Computes the from square for a white pawn attack diagonal right.
std::uint8_t
from_right_white(std::uint8_t to_square) noexcept
{ return to_square - 9; }

// Computes the from square for a one-square black pawn move given the
// destination square.
std::uint8_t
from_single_black(std::uint8_t to_square) noexcept
{ return to_square + 8; }

// Computes the from square for a double-square black pawn move given the
// destination square.
std::uint8_t
from_double_black(std::uint8_t to_square) noexcept
{ return to_square + 16; }

// Computes the from square for a black pawn attack diagonal left.
std::uint8_t
from_left_black(std::uint8_t to_square) noexcept
{ return to_square + 7; }

// Computes the from square for a black pawn attack diagonal right.
std::uint8_t
from_right_black(std::uint8_t to_square) noexcept
{ return to_square + 9; }

// Function to generate forward pawn moves.
void
move_forward(
    BitBoard pawns,
    BitBoard no_pieces,
    PawnMovesFn move_fn,
    FromFn from_fn,
    IsPromoFn is_promo_fn,
    MoveVec& moves)
{
  auto pawn_moves = move_fn(pawns, no_pieces);

  while (pawn_moves) {
    auto to_square = pawn_moves.first_bit_and_clear();
    auto from_square = from_fn(to_square);

    if (not is_promo_fn(to_square))
      moves.emplace_back(Piece::pawn(), from_square, to_square);
    else {
      // Pawn is moving to the last rank for promotion.
      moves.push_back(Move::promo(from_square, to_square, Piece::queen()));
      moves.push_back(Move::promo(from_square, to_square, Piece::rook()));
      moves.push_back(Move::promo(from_square, to_square, Piece::bishop()));
      moves.push_back(Move::promo(from_square, to_square, Piece::knight()));
    }
  }
}

// Function to generate pawn attacks.
void
attack_with_pawns(
    BitBoard pawns,
    Board board,
    PawnMovesFn move_fn,
    FromFn from_fn,
    IsPromoFn is_promo_fn,
    MoveVec& moves)
{
  auto pawn_moves = move_fn(pawns, board.all_other());

  while (pawn_moves) {
    auto [to_square, to_bb] = pawn_moves.index_bb_and_clear();
    auto to_piece = board.other().find_type(to_bb);
    assert(to_piece.type() != Type::None);
    auto from_square = from_fn(to_square);

    if (not is_promo_fn(to_square))
      moves.emplace_back(Piece::pawn(), from_square, to_piece, to_square);
    else {
      moves.push_back(Move::promo(
            from_square, to_piece, to_square, Piece::queen()));
      moves.push_back(Move::promo(
            from_square, to_piece, to_square, Piece::rook()));
      moves.push_back(Move::promo(
            from_square, to_piece, to_square, Piece::bishop()));
      moves.push_back(Move::promo(
            from_square, to_piece, to_square, Piece::knight()));
    }
  }
}

} // namespace

Board
Board::new_board() noexcept
{
  Board board;

  board.bb_mine = PieceSet::init_white();
  board.bb_other = PieceSet::init_black();

  board.half_move = 0;
  board.full_move = 1;

  board.next_to_move = Color::White;

  board.en_passant = false;
  board.en_passant_file = false;

  board.wk_castle = true;
  board.wq_castle = true;
  board.bk_castle = true;
  board.bq_castle = true;

  board.set_attacked_by_mine();
  board.set_attacked_by_other();

  return board;
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
  buff += is_white_next() ? 'w' : 'b';
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

MoveVec
Board::all_moves() const
{
  MoveVec moves;
  pawn_moves(moves);
  knight_moves(moves);
  bishop_moves(moves);
  rook_moves(moves);
  queen_moves(moves);
  king_moves(moves);
  return moves;
}

// TODO: check any moves are illegal. For example:
// - king cannot put himself in check.
// - castling is not possible if one of the squares the king has to cross is
//   attacked.
//
// This can be done while we generating the king moves, to prevent creating a
// move that is not legal, or after the moves are generated. The latter might
// be necessary and simpler to do.
void
Board::king_moves(MoveVec& moves) const
{
  auto move_king_fn = [&](auto bb) -> BitBoard {
    // Make sure that none of the squares where the king is moving are attacked.
    return move_king(bb) & bb_empty_attacks.bit_not();
  };
  get_simple_moves(Piece::king(), move_king_fn, moves);

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
}

void
Board::bishop_moves(MoveVec& moves) const
{
  assert(bmagics);
  auto moves_fn = [&](BitBoard bb) {
    auto from_square = bb.first_bit();
    return bmagics->get_attacks(from_square, all_bits());
  };
  return get_simple_moves(Piece::bishop(), moves_fn, moves);
}

void
Board::rook_moves(MoveVec& moves) const
{
  assert(rmagics);
  auto moves_fn = [&](BitBoard bb) {
    auto from_square = bb.first_bit();
    return rmagics->get_attacks(from_square, all_bits());
  };
  return get_simple_moves(Piece::rook(), moves_fn, moves);
}

void
Board::queen_moves(MoveVec& moves) const
{
  assert(bmagics and rmagics);
  auto moves_fn = [&](BitBoard bb) {
    auto blockers = all_bits();
    auto from_square = bb.first_bit();
    return bmagics->get_attacks(from_square, blockers)
         | rmagics->get_attacks(from_square, blockers);
  };
  return get_simple_moves(Piece::rook(), moves_fn, moves);
}

void
Board::pawn_moves(MoveVec& moves) const
{
  BitBoard pawns = mine().pawn();

  if (not pawns)
    return;

  PawnMovesFn single_fn;
  PawnMovesFn double_fn;
  PawnMovesFn attack_left_fn;
  PawnMovesFn attack_right_fn;
  FromFn from_single_fn;
  FromFn from_double_fn;
  FromFn from_left_fn;
  FromFn from_right_fn;
  IsPromoFn is_promo_fn;

  if (is_white_next()) {
    single_fn = move_wp_single;
    double_fn = move_wp_double;
    attack_left_fn = move_wp_left;
    attack_right_fn = move_wp_right;
    from_single_fn = from_single_white;
    from_double_fn = from_double_white;
    from_left_fn = from_left_white;
    from_right_fn = from_right_white;
    is_promo_fn = is_white_promo;
  } else {
    single_fn = move_bp_single;
    double_fn = move_bp_double;
    attack_left_fn = move_bp_left;
    attack_right_fn = move_bp_right;
    from_single_fn = from_single_black;
    from_double_fn = from_double_black;
    from_left_fn = from_left_black;
    from_right_fn = from_right_black;
    is_promo_fn = is_black_promo;
  }

  auto no_pieces = none();

  move_forward(pawns, no_pieces, single_fn, from_single_fn, is_promo_fn, moves);
  move_forward(pawns, no_pieces, double_fn, from_double_fn, is_promo_fn, moves);
  attack_with_pawns(
      pawns, *this, attack_left_fn, from_left_fn, is_promo_fn, moves);
  attack_with_pawns(
      pawns, *this, attack_right_fn, from_right_fn, is_promo_fn, moves);
  move_enpassant(moves);
}

std::optional<unsigned>
Board::compute_passant_file(Move mv) const noexcept
{
  assert(mv.piece().type() == Type::Pawn);

  // Convert to int here to avoid issues below where we do subtraction.
  auto fs = static_cast<int>(mv.from());
  auto ts = static_cast<int>(mv.to());

  // En passant is only possible when the pawn moves two squares forward.
  if (std::labs(ts - fs) != 16)
    return std::nullopt;

  auto bb = BitBoard::from_index(ts);
  auto neighbors = (move_east(bb) | move_west(bb)) & bb_other.pawn();

  if (neighbors)
    return ts % 8;

  return std::nullopt;
}

void
Board::compute_game_state(bool is_king_captured) noexcept
{
  if (is_king_captured)
    game_state = GameState::Mate;
  else
    compute_game_state();
}

// TODO: need to figure out how to check for 3 move repetition. To check for
// 3-move repetation, it's not actually checking the move, but that the position
// of the board is not repeated three times after both players move. One way of
// doing this would be to keep a history of every position leading up to the
// current position, or enough state to be able to reconstruct it. We could also
// compute a hash from the position to speed up comparison, and only do a full
// state comparison when the hash values are equal.
void
Board::compute_game_state() noexcept
{
  // Draw by 50 move rule.
  if (half_move >= 100) {
    game_state = GameState::Draw;
    return;
  }

  // Draw by insufficient material.
  if (not enough_material()) {
    game_state = GameState::Draw;
    return;
  }

  // We compute the moves and next states to check for stalemate, draw, and
  // check mate.
  auto moves = all_moves();
  if (moves.empty()) {
    // For now, we'll assume this means stalemate.
    game_state = GameState::Draw;
    return;
  }

  if (not is_check()) {
    // We continue playing, i.e. GameState::Playing.
    return;
  }

  for (auto mv : moves) {
    Board board(*this);
    board.quick_update(mv);
    // Found a way to get out of check.
    if (not board.is_check_other()) {
      game_state = GameState::Playing;
      return;
    }
  }

  // Didn't find a move to get out of check.
  game_state = GameState::Mate;
}

Board&
Board::quick_update(Move mv) noexcept
{
  auto from_piece = mv.piece();
  assert(from_piece.type() != Type::None);

  auto from_square = mv.from();
  auto to_square = mv.to();

  bb_mine.update_bit(from_piece, from_square, to_square);

  // En passant capture square is different from square where piece is moving.
  if (mv.is_enpassant())
    bb_other.clear_bit(mv.capture(), mv.passant());
  else if (mv.is_capture())
    bb_other.clear_bit(mv.capture(), to_square);

  if (mv.is_promo()) {
    bb_mine.clear_bit(Piece::pawn(), to_square);
    bb_mine.set_bit(mv.promoted(), to_square);
  }

  if (mv.is_castling()) {
    auto rk_from_to = mv.get_rook_from_to();
    assert(rk_from_to);
    auto [rk_from, rk_to] = *rk_from_to;
    bb_mine.update_bit(Piece::rook(), rk_from, rk_to);
  }

  // Half move is reset on capture or pawn move, and incremented otherwise.
  if (mv.is_capture() or from_piece.type() == Type::Pawn)
    half_move = 0;
  else
    ++half_move;

  // Full move is incremented after black moves.
  if (not is_white_next())
    ++full_move;

  // Reset en passant files.
  en_passant = false;
  en_passant_file = 0;

  if (from_piece.type() == Type::Pawn) {
    auto passant = compute_passant_file(mv);
    if (passant) {
      en_passant = true;
      en_passant_file = *passant;
    }
  } else if (from_piece.type() == Type::Rook) {
    if (is_white_next()) {
      if (from_square == 0)
        wq_castle = false;
      else if (from_square == 7)
        wk_castle = false;
    }
    else if (from_square == 56)
      bq_castle = false;
    else if (from_square == 63)
      bk_castle = false;
  } else if (from_piece.type() == Type::King) {
    if (is_white_next()) {
      wk_castle = false;
      wq_castle = false;
    } else {
      bk_castle = false;
      bq_castle = false;
    }
  }

  bb_mine.swap(bb_other);
  next_to_move = is_white_next() ? Color::Black : Color::White;

  set_attacked_by_mine();
  set_attacked_by_other();

  return *this;
}

Board&
Board::update(Move mv) noexcept
{
  quick_update(mv);
  compute_game_state(mv.capture().is_king());
  return *this;
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

// We create a BitBoard with one bit set where the pawn moves after capturing
// en passant, and use that to check if there are any captures.
void
Board::move_enpassant(MoveVec& moves) const
{
  if (not has_enpassant())
    return;

  // Fake square
  auto pawns = mine().pawn();
  unsigned to_sq = enpassant_file();
  unsigned passant_sq = to_sq;

  if (is_white_next()) {
    to_sq += 40; // 6th row
    passant_sq += 32; // 5th row

    auto to_bb = BitBoard::from_index(to_sq);

    auto attack = move_wp_left(pawns, to_bb);
    if (attack) {
      auto from_sq = from_left_white(to_sq);
      moves.push_back(Move::by_enpassant(from_sq, to_sq, passant_sq));
    }

    attack = move_wp_right(pawns, to_bb);
    if (attack) {
      auto from_sq = from_right_white(to_sq);
      moves.push_back(Move::by_enpassant(from_sq, to_sq, passant_sq));
    }
  } else {
    to_sq += 16; // 3rd row
    passant_sq += 24; // 4th row

    auto to_bb = BitBoard::from_index(to_sq);

    auto attack = move_bp_left(pawns, to_bb);
    if (attack) {
      auto from_sq = from_left_black(to_sq);
      moves.push_back(Move::by_enpassant(from_sq, to_sq, passant_sq));
    }

    attack = move_bp_right(pawns, to_bb);
    if (attack) {
      auto from_sq = from_right_black(to_sq);
      moves.push_back(Move::by_enpassant(from_sq, to_sq, passant_sq));
    }
  }
}

BitBoard
Board::get_attacks(const PieceSet& pieces, BitBoard bb) const noexcept
{
  assert(bmagics and rmagics);

  BitBoard attacked;

  attacked |= move_king(pieces.king()) & bb;
  attacked |= move_knight(pieces.knight()) & bb;

  auto pawns = pieces.pawn();
  if (is_white_next()) {
    attacked |= move_bp_left(pawns, bb);
    attacked |= move_bp_right(pawns, bb);
  } else {
    attacked |= move_wp_left(pawns, bb);
    attacked |= move_wp_right(pawns, bb);
  }

  auto blockers = pieces.full_set();

  for (auto s : pieces.bishop().square_iter())
    attacked |= bmagics->get_attacks(s, blockers) & bb;

  for (auto s : pieces.rook().square_iter())
    attacked |= rmagics->get_attacks(s, blockers) & bb;

  for (auto s : pieces.queen().square_iter()) {
    auto qattacks = bmagics->get_attacks(s, blockers)
                  | rmagics->get_attacks(s, blockers);
    attacked |= qattacks & bb;
  }

  return attacked;
}

AttackSquares
Board::get_attacks_other() const noexcept
{
  assert(bmagics and rmagics);

  AttackSquares attacks;
  attacks.pieces = get_attacks(other(), all_mine());
  attacks.no_pieces = get_attacks(other(), none());
  return attacks;
}

Board&
Board::set_attacked_by_other() noexcept
{
  other_attacks = get_attacks_other();
  return *this;
}

AttackSquares
Board::get_attacks_mine() const noexcept
{
  assert(bmagics and rmagics);

  AttackSquares attacks;
  attacks.pieces = get_attacks(mine(), all_other());
  attacks.no_pieces = get_attacks(mine(), none());
  return attacks;
}

Board&
Board::set_attacked_by_mine() noexcept
{
  mine_attacks = get_attacks_mine();
  return *this;
}

bool
Board::enough_material() const noexcept
{
  assert(mine().king().count() == 1);
  assert(other().king().count() == 1);

  auto my_count = all_mine().count();
  auto other_count = all_other().count();

  // We expect this to be the common case for most of the game.
  if (my_count > 2 or other_count > 2)
    return true;

  if (my_count == 1 and other_count == 1)
    return false;

  if (my_count == 2 and other_count == 1)
    return mine().bishop().count() == 1
        or mine().knight().count() == 1;

  if (other_count == 2 and my_count == 1)
    return other().bishop().count() == 1
        or other().knight().count() == 1;

  return true;
}

} // namespace blunder
