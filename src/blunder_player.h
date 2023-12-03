#pragma once

#include <cassert>
#include <memory>
#include <string_view>
#include <utility>

#include "board_path.h"
#include "player.h"
#include "search.h"
#include "search_result.h"

namespace blunder {

class BlunderPlayer : public Player {
public:
  explicit
  BlunderPlayer(std::shared_ptr<Search> mcts)
    : mcts(std::move(mcts))
  {
    assert(this->mcts);
  }

  SearchResult
  make_move(const GameBoardPath& boards) override;

  std::string_view
  name() const noexcept
  { return "Blunder"; }

private:
  std::shared_ptr<Search> mcts;
};

} // namespace  blunder
