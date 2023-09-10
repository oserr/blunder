#pragma once

#include "move.h"

namespace blunder {

// Encodes mv as a number in the range [0, 72].
unsigned
encode_move(Move mv);

} // namespace blunder
