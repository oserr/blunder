#pragma once

#include <span>
#include <string_view>

#include "board.h"

namespace blunder {

class Player {
public:
  virtual Board
  make_move(std::span<const Board> boards) = 0;

  virtual std::string_view
  name() const noexcept = 0;
};

} // namespace blunder
