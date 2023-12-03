#include <cstdlib>
#include <format>
#include <getopt.h>
#include <iostream>
#include <memory>
#include <random>
#include <string>
#include <string_view>
#include <unistd.h>

#include "alpha_zero_decoder.h"
#include "alpha_zero_encoder.h"
#include "alpha_zero_evaluator.h"
#include "board.h"
#include "blunder_player.h"
#include "mcts.h"
#include "net.h"
#include "simple_game.h"

using namespace blunder;

void
print_help(std::string_view prog, std::ostream& os)
{
  os << "Usage: " << prog << " [options] ...\n"
     << "   -h|--help   Print this help message.\n"
     << "   -g|--games  The totla number of games to train for.\n"
     << std::endl;
}

int
main(int argc, char** argv)
{
  struct option longopts[] = {
    {"help", no_argument, nullptr, 'h'},
    {"games", required_argument, nullptr, 'g'},
    {0, 0, 0, 0},
  };

  unsigned num_games = 0;

  while (true) {
    auto ret = getopt_long(argc, argv, "hg:", longopts, nullptr);
    if (ret == -1) break;
    switch (ret) {
      case 'h':
        print_help(argv[0], std::cout);
        return EXIT_SUCCESS;
      case 'g':
        try {
          num_games = std::stol(optarg);
        } catch (...) {
          std::cerr << "--games needs to be a valid number greather than 0"
              << " but got " << optarg << std::endl;
          print_help(argv[0], std::cout);
          return EXIT_FAILURE;
        }
        break;
      default:
        std::cerr << "Received unknown command line option.\n";
        print_help(argv[0], std::cerr);
        return EXIT_FAILURE;
    }
  }

  auto net = std::make_shared<AlphaZeroNet>();

  auto evaluator = std::make_shared<AlphaZeroEvaluator>(
          std::move(net),
          std::make_shared<AlphaZeroDecoder>(),
          std::make_shared<AlphaZeroEncoder>());

  auto mcts = std::make_shared<Mcts>(std::move(evaluator), 300, 0);

  Board::register_magics();

  std::cout << "Playing " << num_games << " of self play ..." << std::endl;
  for (unsigned j = 0; j < num_games; ++j) {
    auto game = SimpleGame(
            std::make_unique<BlunderPlayer>(mcts),
            std::make_unique<BlunderPlayer>(mcts));

    auto result = game.play();

    if (not result.winner)
        std::cout << "Game " << j << " ended in a draw." << std::endl;
    else {
        std::cout << "Game " << j << " ended with "
                  << (*result.winner == Color::White ? "WHITE" : "BLACK")
                  << " as winner in " << result.moves.size()
                  << '!' << std::endl;
    }

    std::cout << result.stats().dbg() << std::endl;
  }

  return EXIT_SUCCESS;
}
