#include "move.h"

#include "pieces.h"
#include "square.h"

namespace blunder {

bool
Move::eq(Move mv) const noexcept
{
  return from_piece == mv.from_piece
     and to_piece == mv.to_piece
     and from_square == mv.from_square
     and to_square == mv.to_square
     and castle == mv.castle
     and kside == mv.kside
     and en_passant == mv.en_passant
     and is_promo == mv.is_promo
     and promo_piece == mv.promo_piece;
}

std::string
Move::str() const
{
  std::string buff;
  buff.reserve(128);

  buff += '{';
  buff += Piece::from_int(from_piece).letter();
  buff += ':';
  buff += to_sq_str(from_square);
  buff += "->";
  buff += to_sq_str(to_square);

  if (castle) {
    std::uint8_t from_sq = from_square;
    std::uint8_t to_sq = to_square;
    if (kside) {
      from_sq += 3;
      --to_sq;
    } else {
      from_sq -= 4;
      ++to_sq;
    }

    buff += ", ";
    buff += letter(Type::Rook);
    buff += ':';
    buff += to_sq_str(from_sq);
    buff += "->";
    buff += to_sq_str(to_sq);
  }

  if (auto piece = Piece::from_int(to_piece); piece != Type::None) {
    buff += ", !";
    buff += piece.letter();
  }

  if (is_promo) {
    buff += ", ^";
    buff += Piece::from_int(promo_piece).letter();
  }

  buff += '}';

  return buff;
}

} // namespace blunder
