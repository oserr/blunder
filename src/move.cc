#include "move.h"

namespace blunder {

bool
operator==(const Move& left, const Move& right) noexcept
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

} // namespace blunder
