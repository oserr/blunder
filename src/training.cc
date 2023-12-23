#include <cstdlib>
#include <getopt.h>
#include <iostream>
#include <string_view>
#include <unistd.h>

#include "board.h"
#include "trainer_builder.h"

using namespace blunder;

void
print_help(std::string_view prog, std::ostream& os)
{
  os << "Usage: " << prog << " [options] ...\n"
     << "   -h|--help            Print this help message.\n"
     << "   -t|--training_games  The total number of games to train for.\n"
     << std::endl;
}

int
main(int argc, char** argv)
{
  struct option longopts[] = {
    {"help", no_argument, nullptr, 'h'},
    {"training_games", required_argument, nullptr, 't'},
    {0, 0, 0, 0},
  };

  unsigned training_games = 32;

  while (true) {
    auto ret = getopt_long(argc, argv, "ht:", longopts, nullptr);
    if (ret == -1) break;
    switch (ret) {
      case 'h':
        print_help(argv[0], std::cout);
        return EXIT_SUCCESS;
      case 't':
        try {
          training_games = std::stol(optarg);
        } catch (...) {
          std::cerr << "--training_games needs to be a valid number greather than 0"
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

  Board::register_magics();

  try {
    TrainerBuilder()
      .set_training_sessions(2)
      .set_training_games(training_games)
      .set_training_epochs(3)
      .set_tournament_games(3)
      .set_checkpoint_steps(1)
      .set_batch_size(5)
      .build()
      .train();
  } catch (std::exception& err) {
    std::cerr << err.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
