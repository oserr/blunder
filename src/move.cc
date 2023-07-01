#include "move.h"

#include "pieces.h"
#include "square.h"

namespace blunder {

bool
operator==(Move left, Move right) noexcept
{
  return left.from_piece == right.from_piece
     and left.to_piece == right.to_piece
     and left.from_square == right.from_square
     and left.to_square == right.to_square
     and left.castle == right.castle
     and left.kside == right.kside
     and left.en_passant == right.en_passant
     and left.is_promo == right.is_promo
     and left.promo_piece == right.promo_piece;
}

std::string
DebugStr(Move mv)
{
  std::string buff;
  buff.reserve(128);

  buff += '{';
  buff += Piece::from_int(mv.from_piece).letter();
  buff += ':';
  buff += ToStr(ToSq(mv.from_square));
  buff += "->";
  buff += ToStr(ToSq(mv.to_square));

  if (mv.castle) {
    std::uint8_t from_sq = mv.from_square;
    std::uint8_t to_sq = mv.to_square;
    if (mv.kside) {
      from_sq += 3;
      --to_sq;
    } else {
      from_sq -= 4;
      ++to_sq;
    }

    buff += ", ";
    buff += letter(Type::Rook);
    buff += ':';
    buff += ToStr(ToSq(from_sq));
    buff += "->";
    buff += ToStr(ToSq(to_sq));
  }

  if (auto to_piece = Piece::from_int(mv.to_piece); to_piece != Type::None) {
    buff += ", !";
    buff += to_piece.letter();
  }

  if (mv.is_promo) {
    buff += ", ^";
    buff += Piece::from_int(mv.promo_piece).letter();
  }

  buff += '}';

  return buff;
}

} // namespace blunder
