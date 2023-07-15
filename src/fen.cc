#include "fen.h"

#include <algorithm>
#include <array>
#include <cassert>
#include <cctype>
#include <charconv>
#include <expected>
#include <functional>
#include <iostream>
#include <tuple>
#include <utility>

#include "pieces.h"
#include "piece_set.h"

namespace blunder {
namespace {

namespace rng = std::ranges;

// A structure for parsing the castling rights.
struct Castling {
  bool wking = false;
  bool wqueen = false;
  bool bking = false;
  bool bqueen = false;
};

// Used internally to parse a FEN string.
struct NextField {
  // The next field in the FEN string.
  std::string_view field{};
  // The remaining bytes in the FEN string after the field.
  std::string_view chunk{};
};

// Simple wrappers around std utilities with correct input type.

inline bool
is_space(unsigned char c) noexcept
{ return std::isspace(c); }

inline bool
is_upper(unsigned char c) noexcept
{ return std::isupper(c); }

inline char
to_lower(unsigned char c) noexcept
{ return std::tolower(c); }

// Returns a NextField where field is a string without any spaces, and chunk is
// the remaning part of the string. If there are no more fields, returns an err
// or for missing a field.
std::expected<NextField, FenErr>
get_next_field(std::string_view chunk) noexcept
{
  while (not chunk.empty() and is_space(chunk[0]))
    chunk.remove_prefix(1);

  auto iter = rng::find_if(chunk, [](unsigned char c) { return is_space(c); });

  if (iter == chunk.end() and chunk.empty())
    return std::unexpected(FenErr::MissingField);
  else if (iter == chunk.end())
    return NextField{chunk};

  auto index = iter - chunk.begin();

  return NextField{chunk.substr(0, index), chunk.substr(index)};
}

// Splits the rows in the field containing the pieces, where / splits the rows.
// The rows are returned in reverse order, i.e. 1, 2, 3, ..., instead of 8, 7,
// 6, ..., which is how they appear in a FEN string. Returns an error if we are
// unable to split 8 rows and consume all the bytes.
std::expected<std::array<std::string_view, 8>, FenErr>
split_rows(std::string_view pieces) noexcept
{
  std::array<std::string_view, 8> rows;
  unsigned num_rows = 0;

  while (!pieces.empty() and num_rows <= 8) {
    auto index = pieces.find('/');
    auto row = pieces.substr(0, index);

    if (row.empty() or row.size() > 8)
      return std::unexpected(FenErr::InvalidRow);

    rows[num_rows++] = row;

    if (index == std::string_view::npos)
      pieces.remove_prefix(pieces.size());
    else {
      index = std::min(index+1, pieces.size());
      pieces.remove_prefix(index);
    }
  }

  // We expect to consume everything.
  if (!pieces.empty())
    return std::unexpected(FenErr::ExtraRows);

  // We expect to consume 8 rows.
  if (num_rows != 8)
    return std::unexpected(FenErr::MissingRows);

  rng::reverse(rows);
  return rows;
}

std::expected<std::pair<PieceSet, PieceSet>, FenErr>
parse_pieces(std::string_view field) noexcept
{
  auto rows = split_rows(field);
  if (not rows)
    return std::unexpected(rows.error());

  PieceSet white;
  PieceSet black;

  // Current square being processed.
  unsigned square = 0;
  for (auto row : *rows) {
    for (auto letter : row) {

      if (letter >= '1' and letter <= '8') {
        square += letter - '0';
        continue;
      }

      Piece piece;

      switch (to_lower(letter)) {
        case 'k':
           piece = Type::King;
           break;
        case 'q':
           piece = Type::Queen;
           break;
        case 'r':
           piece = Type::Rook;
           break;
        case 'b':
           piece = Type::Bishop;
           break;
        case 'n':
           piece = Type::Knight;
           break;
        case 'p':
           piece = Type::Pawn;
           break;
        default:
           return std::unexpected(FenErr::UnknownPiece);
      }

      if (is_upper(letter))
        white.set_bit(piece, square);
      else
        black.set_bit(piece, square);

      ++square;
    }
  }

  if (square != 64)
    return std::unexpected(FenErr::Not64Squares);

  if (not white.is_valid())
    return std::unexpected(FenErr::WhiteNotLogical);

  if (not black.is_valid())
    return std::unexpected(FenErr::BlackNotLogical);

  return std::make_pair(white, black);
}

std::expected<Color, FenErr>
parse_color(std::string_view field) noexcept
{
  if (field.size() != 1)
    return std::unexpected(FenErr::InvalidColor);

  switch (field[0]) {
    case 'w':
      return Color::White;
    case 'b':
      return Color::Black;
    default:
      return std::unexpected(FenErr::InvalidColor);
  }
}

std::expected<Castling, FenErr>
parse_castling(std::string_view field) noexcept
{
  if (field.size() < 1 or field.size() > 4)
    return std::unexpected(FenErr::InvalidCastling);

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
        return std::unexpected(FenErr::InvalidCastling);
    }
  }

  return castling;
}

std::expected<std::optional<unsigned>, FenErr>
parse_enpassant(std::string_view field) noexcept
{
  switch (field.size()) {
    case 1:
      if (field[0] == '-')
        return std::nullopt;
      break;
    case 2: {
      auto letter = to_lower(field[0]);
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
  return std::unexpected(FenErr::InvalidEnPassant);
}

std::expected<unsigned, FenErr>
parse_num(std::string_view field) noexcept
{
  if (field.empty())
    return std::unexpected(FenErr::InvalidNum);

  unsigned num;
  auto [ptr, from_err] = std::from_chars(field.begin(), field.end(), num);

  if (from_err == std::errc())
    return num;

  return std::unexpected(FenErr::InvalidNum);
}

enum class FieldType {
  Pieces,
  Color,
  Castling,
  EnPassant,
  HalfMove,
  FullMove
};

std::unexpected<FenErr>
missing_field(FieldType field_type) noexcept
{
  switch (field_type) {
    case FieldType::Pieces:
      return std::unexpected(FenErr::NoPieces);
    case FieldType::Color:
      return std::unexpected(FenErr::NoColor);
    case FieldType::Castling:
      return std::unexpected(FenErr::NoCastling);
    case FieldType::EnPassant:
      return std::unexpected(FenErr::NoEnPassant);
    case FieldType::HalfMove:
      return std::unexpected(FenErr::NoHalfMove);
    case FieldType::FullMove:
      return std::unexpected(FenErr::NoFullMove);
    default:
      assert(false and "Unexpected value for FieldType.");
  }
  return std::unexpected(FenErr::Internal);
}

} // namespace

std::expected<BoardState, FenErr>
read_fen(std::string_view fen) noexcept
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
    auto next_field = get_next_field(fen);

    if (not next_field)
      return missing_field(field_type);

    auto [field, chunk] = *next_field;

    switch (field_type) {
      case FieldType::Pieces: {
        auto pieces = parse_pieces(field);
        if (not pieces)
          return std::unexpected(pieces.error());
        std::tie(white, black) = *pieces;
        break;
      }
      case FieldType::Color: {
        auto color_val = parse_color(field);
        if (not color_val)
          return std::unexpected(color_val.error());
        color = *color_val;
        break;
      }
      case FieldType::Castling: {
        auto castling_val = parse_castling(field);
        if (not castling_val)
          return std::unexpected(castling_val.error());
        castling = *castling_val;
        break;
      }
      case FieldType::EnPassant: {
        auto en_passant_val = parse_enpassant(field);
        if (not en_passant_val)
          return std::unexpected(en_passant_val.error());
        if (*en_passant_val)
          en_passant = **en_passant_val;
        break;
      }
      case FieldType::HalfMove: {
        auto num = parse_num(field);
        if (not num)
          return std::unexpected(FenErr::InvalidHalfMove);
        if (*num > 100)
          return std::unexpected(FenErr::InvalidHalfMove);
        half_move = *num;
        break;
      }
      case FieldType::FullMove: {
        auto num = parse_num(field);
        if (not num)
          return std::unexpected(FenErr::InvalidFullMove);
        full_move = *num;
        break;
      }
    }

    // process the remaining chunk
    fen = chunk;
  }

  BoardStateBuilder builder;
  if (en_passant)
    builder.set_enpassant_file(*en_passant % 8);

  auto state = builder
         .set_pieces(color, white, black)
         .set_wk_castling(castling.wking)
         .set_wq_castling(castling.wqueen)
         .set_bk_castling(castling.bking)
         .set_bq_castling(castling.bqueen)
         .set_half_move(half_move)
         .set_full_move(full_move)
         .build();

  if (not state)
    return std::unexpected(FenErr::Internal);

  return *state;
}

std::string_view
str_view(FenErr err) noexcept
{
  switch (err) {
    case FenErr::NoPieces:
      return "NoPieces";
    case FenErr::NoColor:
      return "NoColor";
    case FenErr::NoCastling:
      return "NoCastling";
    case FenErr::NoEnPassant:
      return "NoEnPassant";
    case FenErr::NoHalfMove:
      return "NoHalfMove";
    case FenErr::NoFullMove:
      return "NoFullMove";
    case FenErr::InvalidColor:
      return "InvalidColor";
    case FenErr::InvalidCastling:
      return "InvalidCastling";
    case FenErr::InvalidEnPassant:
      return "InvalidEnPassant";
    case FenErr::InvalidHalfMove:
      return "InvalidHalfMove";
    case FenErr::InvalidFullMove:
      return "InvalidFullMove";
    case FenErr::MissingField:
      return "MissingField";
    case FenErr::UnknownPiece:
      return "UnknownPiece";
    case FenErr::WhiteNotLogical:
      return "WhiteNotLogical";
    case FenErr::BlackNotLogical:
      return "BlackNotLogical";
    case FenErr::Not64Squares:
      return "Not64Squares";
    case FenErr::InvalidRow:
      return "InvalidRow";
    case FenErr::MissingRows:
      return "MissingRows";
    case FenErr::ExtraRows:
      return "ExtraRows";
    case FenErr::InvalidNum:
      return "InvalidNum";
    default:
      assert(false and "Unexpected value for FenErr.");
  }
  return "UNKNOWN_ENUM_VALUE";
}

} // namespace blunder
