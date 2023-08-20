#include "terminal_player.h"

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>

#include "board.h"
#include "board_path.h"

namespace blunder {

Board
TerminalPlayer::make_move(std::span<const Board> boards)
{
  if (boards.empty())
    throw std::invalid_argument("Boards should not be empty.");

  const auto& board = boards.back();
  auto next_boards = board.next();

  if (next_boards.empty())
    throw std::logic_error("Did not find any moves.");

  std::cout << board << '\n';

  unsigned i = 0;
  std::cout << "\nChoose a move...\n";
  for (const auto& next_board : next_boards) {
    auto mv = next_board.last_move();
    assert(mv);
    std::cout << '\t' << i++ << " : " << *mv << '\n';
  }

  std::cout << "Enter a number for a move: " << std::endl;

  unsigned index = 0;

  while (true) {
    bool got_index = false;
    std::string index_str;
    std::getline(std::cin, index_str);
    try {
      index = std::stoul(index_str);
      got_index = true;
    } catch (...) { }

    if (got_index and index < next_boards.size())
      break;

    std::cout << "Invalid option. Number should be between 0 and "
              << next_boards.size()
              << "\nEnter a number for a move: "
              << std::endl;
  }

  return next_boards[index];
}

} // namespace blunder
