#include <cstdlib>
#include <getopt.h>
#include <iostream>
#include <random>
#include <string_view>
#include <unistd.h>

#include "board.h"
#include "random_player.h"
#include "simple_game.h"
#include "terminal_player.h"

using namespace blunder;

void
print_help(std::string_view prog, std::ostream& os)
{
  os << "Usage: " << prog << " [options] ...\n"
     << "   -h|--help   Print this help message.\n"
     << "   -p|--pname  The name of the player, or TerminalGame by default.\n"
     << "   -w|--white  Play the game as white. Exclusive of --black.\n"
     << "   -b|--black  Play the game as black. Exclusive of --white.\n"
     << std::endl;
}

int
main(int argc, char** argv)
{

  struct option longopts[] = {
    {"help", no_argument, nullptr, 'h'},
    {"pname", required_argument, nullptr, 'n'},
    {"white", no_argument, nullptr, 'w'},
    {"black", no_argument, nullptr, 'b'},
    {0, 0, 0, 0},
  };

  std::string_view player_name = "TerminalPlayer";
  bool as_white = false;
  bool as_black = false;

  while (true) {
    auto ret = getopt_long(argc, argv, "hp", longopts, nullptr);
    if (ret == -1) break;
    switch (ret) {
      case 'h':
        print_help(argv[0], std::cout);
        return EXIT_SUCCESS;
      case 'p':
        player_name = optarg;
        break;
      case 'w':
        as_white = true;
        break;
      case 'b':
        as_black = true;
        break;
      default:
        std::cerr << "Received unknown command line option.\n";
        print_help(argv[0], std::cerr);
        return EXIT_FAILURE;
    }
  }

  if (as_white and as_black) {
    std::cerr << "Only 1 of -w or -b should be specified, if one is specified."
              << std::endl;
    return EXIT_FAILURE;
  }

  std::random_device rand_dev;

  auto terminal_player = std::make_unique<TerminalPlayer>(player_name);
  auto rand_player = std::make_unique<RandomPlayer>(rand_dev());

  bool is_white = false;
  if (as_white or as_black)
    is_white = as_white ? true : false;
  else {
    std::mt19937 gen(rand_dev());
    std::bernoulli_distribution dist(0.5);
    // Randomly choose to play as white or black.
    is_white = dist(gen);
  }

  auto game = is_white
     ? SimpleGame(std::move(terminal_player), std::move(rand_player))
     : SimpleGame(std::move(rand_player), std::move(terminal_player));

  Board::register_magics();

  auto results = game.play();

  if (results.winner) {
    std::cout << "The game ended with "
              << (*results.winner == Color::White ? "white" : "black")
              << " as winner!" << std::endl;
  } else {
    std::cout << "The game ended in a draw!" << std::endl;
  }

  return EXIT_SUCCESS;
}
