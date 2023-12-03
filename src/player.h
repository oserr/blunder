#pragma once

#include <span>
#include <string_view>

#include "board.h"
#include "board_path.h"
#include "search_result.h"

namespace blunder {

class Player {
public:
  // Default dtor for Player.
  virtual ~Player() = default;

  virtual SearchResult
  make_move(const GameBoardPath& boards) = 0;

  virtual std::string_view
  name() const noexcept = 0;
};

} // namespace blunder
