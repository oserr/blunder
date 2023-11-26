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
  presult.num_expanded = result.num_expanded;
  presult.depth = result.depth;
  presult.millis_per_eval = result.millis_per_eval;
  presult.millis_eval = result.millis_eval;
  presult.millis_search_time = result.millis_search_time;
  presult.other_moves = std::move(result.moves);
  return presult;
}

} // namespace blunder
