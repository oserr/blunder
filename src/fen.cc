#include "fen.h"

#include <algorithm>
#include <cassert>
#include <cctype>
#include <charconv>
#include <expected>
#include <tuple>
#include <utility>

#include "pieces.h"

namespace blunder {
namespace {

using std::ranges::find_if;

// A structure for parsing the castling rights.
struct Castling {
  bool wking = false;
  bool wqueen = false;
  bool bking = false;
  bool bqueen = false;
};

// Used internally to parse to parse a FEN string.
struct NextField {
  // The next field in the FEN string.
  std::string_view field{};
  // The remaining bytes in the FEN string after the field.
  std::string_view chunk{};
};

// Simple wrappers around std utilities with correct input type.

inline bool
IsSpace(unsigned char c) noexcept
{ return std::isspace(c); }

inline bool
IsUpper(unsigned char c) noexcept
{ return std::isupper(c); }

inline char
ToLower(unsigned char c) noexcept
{ return std::tolower(c); }

// Returns a NextField where field is a string without any spaces, and chunk is
// the remaning part of the string. If there are no more fields, returns an err
// or for missing a field.
std::expected<NextField, FenErr>
GetNextField(std::string_view chunk) noexcept
{
  while (not chunk.empty() and IsSpace(chunk[0]))
    chunk.remove_prefix(1);

  auto iter = find_if(chunk, [](unsigned char c) { return IsSpace(c); });

  if (iter == chunk.end() and chunk.empty())
    return std::unexpected(FenErr::MissingField);
  else if (iter == chunk.end())
    return NextField{chunk};
  
  auto index = iter - chunk.begin();

  return NextField{chunk.substr(0, index), chunk.substr(index)};
}

std::expected<std::pair<PieceSet, PieceSet>, FenErr>
ParsePieces(std::string_view field) noexcept
{
  PieceSet white;
  PieceSet black;
  white.fill(0ull);
  black.fill(0ull);

  auto iter = field.begin();
  auto last = field.end();

  // Use this to check that we processed 64 squares at the end of the loops.
  int num_squares = 0;
  for (int r = 7; r >= 0 and iter != last; --r) {
    for (int f = r * 8, l = f + 8; iter != last and f < l; ++iter) {
      auto letter = *iter;

      if (letter == '/')
        break;

      if (letter >= '1' and letter <= '8') {
        auto num_empty = letter - '0';
        f += num_empty;
        num_squares += num_empty;
        continue;
      }

      Piece piece;

      switch (ToLower(letter)) {
        case 'k':
           piece = Piece::King;
           break;
        case 'q':
           piece = Piece::Queen;
           break;
        case 'r':
           piece = Piece::Rook;
           break;
        case 'b':
           piece = Piece::Bishop;
           break;
        case 'n':
           piece = Piece::Knight;
           break;
        case 'p':
           piece = Piece::Pawn;
           break;
        default:
           return std::unexpected(FenErr::Pieces);
      }

      if (IsUpper(letter))
        white[Uint8(piece)] |= 1ull << f;
      else
        black[Uint8(piece)] |= 1ull << f;

      ++f;
      ++num_squares;
    }
  }

  if (num_squares != 64)
    return std::unexpected(FenErr::Pieces);

  return std::make_pair(white, black);
}

std::expected<Color, FenErr>
ParseColor(std::string_view field) noexcept
{
  if (field.size() != 1)
    return std::unexpected(FenErr::Color);

  switch (field[0]) {
    case 'w':
      return Color::White;
    case 'b':
      return Color::Black;
    default:
      return std::unexpected(FenErr::Color);
  }
}

std::expected<Castling, FenErr>
ParseCastling(std::string_view field) noexcept
{
  if (field.size() < 1 or field.size() > 4)
    return std::unexpected(FenErr::Castling);

  Castling castling;

  if (field.size() == 1 and field[0] == '-')
    return castling;

  for (auto letter : field) {
    switch (letter) {
      case 'K':
        castling.wking = true;
        break;
      case 'Q':
        castling.wqueen = true;
        break;
      case 'k':
        castling.bking = true;
        break;
      case 'q':
        castling.bqueen = true;
        break;
      default:
        return std::unexpected(FenErr::Castling);
    }
  }

  return castling;
}

std::expected<std::optional<unsigned>, FenErr>
ParseEnPassant(std::string_view field) noexcept
{
  switch (field.size()) {
    case 1:
      if (field[0] == '-')
        return std::nullopt;
      break;
    case 2: {
      auto letter = ToLower(field[0]);
      if (letter < 'a' or letter > 'h')
        break;

      auto col = letter - 'a';
      letter = field[1];
      if (letter < '1' or letter > '8')
        break;

      auto row = letter - '1';
      return row * 8 + col;
    }
    default:
      break;
  }
  return std::unexpected(FenErr::EnPassant);
}

std::expected<unsigned, FenErr>
ParseNumber(std::string_view field, FenErr err) noexcept
{
  if (field.empty())
    return std::unexpected(err);

  unsigned num_moves;
  auto first = field.data();
  auto last = first + field.size();

  auto [ptr, from_err] = std::from_chars(first, last, num_moves);

  if (from_err == std::errc())
    return num_moves;

  return std::unexpected(err);
}

enum class FieldType {
  Pieces,
  Color,
  Castling,
  EnPassant,
  HalfMove,
  FullMove
};

} // namespace

std::expected<BoardState, FenErr>
ReadFen(std::string_view fen) noexcept
{
  FieldType fields[] = {
    FieldType::Pieces,
    FieldType::Color,
    FieldType::Castling,
    FieldType::EnPassant,
    FieldType::HalfMove,
    FieldType::FullMove
  };

  PieceSet white;
  PieceSet black;
  Color color = Color::White;
  Castling castling;
  std::optional<unsigned> en_passant;
  unsigned half_move = 0;
  unsigned full_move = 0;

  for (auto field_type : fields) {
    auto next_field = GetNextField(fen);

    if (not next_field)
      return std::unexpected(next_field.error());

    auto [field, chunk] = *next_field;

    switch (field_type) {
      case FieldType::Pieces: {
        auto pieces = ParsePieces(field);
        if (not pieces)
          return std::unexpected(pieces.error());
        std::tie(white, black) = *pieces;
        break;
      }
      case FieldType::Color: {
        auto color_val = ParseColor(field);
        if (not color_val)
          return std::unexpected(color_val.error());
        color = *color_val;
        break;
      }
      case FieldType::Castling: {
        auto castling_val = ParseCastling(field);
        if (not castling_val)
          return std::unexpected(castling_val.error());
        castling = *castling_val;
        break;
      }
      case FieldType::EnPassant: {
        auto en_passant_val = ParseEnPassant(field);
        if (not en_passant_val)
          return std::unexpected(en_passant_val.error());
        if (en_passant_val->has_value())
          en_passant = **en_passant_val;
        break;
      }
      case FieldType::HalfMove: {
        auto num = ParseNumber(field, FenErr::HalfMove);
        if (not num)
          return std::unexpected(num.error());
        half_move = *num;
        break;
      }
      case FieldType::FullMove: {
        auto num = ParseNumber(field, FenErr::FullMove);
        if (not num)
          return std::unexpected(num.error());
        full_move = *num;
        break;
      }
    }

    // process the remaining chunk
    fen = chunk;
  }

  BoardState state;

  if (color == Color::White) {
    state.mine = white;
    state.other = black;
  } else {
    state.mine = black;
    state.other = white;
  }

  state.next = color;
  state.wk_castle = castling.wking;
  state.wq_castle = castling.wqueen;
  state.bk_castle = castling.bking;
  state.bq_castle = castling.bqueen;
  state.half_move = half_move;
  state.full_move = full_move;

  if (en_passant) {
    state.en_passant = 1;
    state.en_passant_file = *en_passant % 8;
  }

  state.SetAllMine();
  state.SetAllOther();

  return state;
}

} // namespace blunder
