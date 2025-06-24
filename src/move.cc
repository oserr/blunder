#include "move.h"

#include "pieces.h"
#include "square.h"

namespace blunder {

std::string
Move::str() const
{
  std::string buff;
  buff.reserve(128);

  buff += '{';
  buff += piece().letter();
  buff += ':';
  buff += to_sq_str(from_square);
  buff += "->";
  buff += to_sq_str(to_square);

  if (is_castling()) {
    std::uint8_t from_sq = from_square;
    std::uint8_t to_sq = to_square;
    if (type() == MoveType::KingCastle) {
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

  if (is_capture()) {
    buff += ", !";
    buff += to_piece->letter();
  }

  if (is_promo()) {
    buff += ", ^";
    buff += promo_piece->letter();
  }

  buff += '}';

  return buff;
}

std::optional<std::pair<unsigned, unsigned>>
Move::get_rook_from_to() const noexcept
{
  if (is_kcastling())
    return std::make_pair(from_square + 3, to_square - 1);
  else if (is_qcastling())
    return std::make_pair(from_square - 4, to_square + 1);
  else
    return std::nullopt;
}

} // namespace blunder
