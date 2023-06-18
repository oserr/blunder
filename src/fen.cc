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

  auto iter = rng::find_if(chunk, [](unsigned char c) { return IsSpace(c); });

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
SplitRows(std::string_view pieces) noexcept
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

// Checks the following:
// - only 1 king
// - no more than 16 total pieces
// - no more than 8 pawns
// - no more than 10 rooks
// - no more than 10 bishops
// - no more than 10 knights
// - no more than 9 queens
bool
AreLogical(const PieceSet pieces) noexcept
{
  return std::popcount(pieces[Uint8(Piece::King)]) == 1
     and std::popcount(pieces[Uint8(Piece::Queen)]) <= 9
     and std::popcount(pieces[Uint8(Piece::Rook)]) <= 10
     and std::popcount(pieces[Uint8(Piece::Bishop)]) <= 10
     and std::popcount(pieces[Uint8(Piece::Knight)]) <= 10
     and std::popcount(pieces[Uint8(Piece::Pawn)]) <= 10
     and std::popcount(AllMask(pieces)) <= 16;
}

std::expected<std::pair<PieceSet, PieceSet>, FenErr>
ParsePieces(std::string_view field) noexcept
{
  PieceSet white;
  PieceSet black;
  white.fill(0ull);
  black.fill(0ull);

  auto rows = SplitRows(field);
  if (not rows)
    return std::unexpected(rows.error());

  // Current square being processed.
  unsigned square = 0;
  for (auto row : *rows) {
    for (auto letter : row) {

      if (letter >= '1' and letter <= '8') {
        square += letter - '0';
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
           return std::unexpected(FenErr::UnknownPiece);
      }

      if (IsUpper(letter))
        white[Uint8(piece)] |= 1ull << square;
      else
        black[Uint8(piece)] |= 1ull << square;

      ++square;
    }
  }

  if (square != 64)
    return std::unexpected(FenErr::Not64Squares);

  if (not AreLogical(white))
    return std::unexpected(FenErr::WhiteNotLogical);

  if (not AreLogical(black))
    return std::unexpected(FenErr::BlackNotLogical);

  return std::make_pair(white, black);
}

std::expected<Color, FenErr>
ParseColor(std::string_view field) noexcept
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
ParseCastling(std::string_view field) noexcept
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
  return std::unexpected(FenErr::InvalidEnPassant);
}

std::expected<unsigned, FenErr>
ParseNumber(std::string_view field) noexcept
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
MissingField(FieldType field_type) noexcept
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
      return MissingField(field_type);

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
        auto num = ParseNumber(field);
        if (not num)
          return std::unexpected(FenErr::InvalidHalfMove);
        if (*num > 100)
          return std::unexpected(FenErr::InvalidHalfMove);
        half_move = *num;
        break;
      }
      case FieldType::FullMove: {
        auto num = ParseNumber(field);
        if (not num)
          return std::unexpected(FenErr::InvalidFullMove);
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

std::string_view
StrView(FenErr err) noexcept
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
