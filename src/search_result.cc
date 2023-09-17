#include "search_result.h"

#include <algorithm>
#include <iterator>
#include <utility>
#include <vector>

namespace blunder {

PlayResult
PlayResult::take_from(SearchResult result)
{
  PlayResult presult;
  presult.play_move = std::move(result.best);
  presult.value = result.value;
  presult.other_moves.reserve(result.moves.size());
  std::ranges::transform(
      result.moves,
      std::back_inserter(presult.other_moves),
      &MoveProb::from);
  return presult;
}

} // namespace blunder
