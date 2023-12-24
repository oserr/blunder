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
     << "   -h|--help               Print this help message.\n"
     << "   -t|--training_games     The total number of games to train for.\n"
     << "   -s|--training_sessions  The total number of training sessions.\n"
     << "   -e|--training_epochs    The total number of training epochs.\n"
     << "   -g|--tournament_games   The total number of tournament_games.\n"
     << "   -b|--batch_size         The number of examples to use per batch.\n"
     << "   -c|--checkpoint_steps   Number of steps before creating a checkpoint.\n"
     << std::endl;
}

int
main(int argc, char** argv)
{
  struct option longopts[] = {
    {"help", no_argument, nullptr, 'h'},
    {"training_games", required_argument, nullptr, 't'},
    {"training_sessions", required_argument, nullptr, 's'},
    {"training_epochs", required_argument, nullptr, 'e'},
    {"tournament_games", required_argument, nullptr, 'g'},
    {"batch_size", required_argument, nullptr, 'b'},
    {"checkpoint_steps", required_argument, nullptr, 'c'},
    {0, 0, 0, 0},
  };

  unsigned training_games = 30;
  unsigned batch_size = 32;
  unsigned training_sessions = 3;
  unsigned training_epochs = 10;
  unsigned tournament_games = 20;
  unsigned checkpoint_steps = 10;

  // TODO: factor out some of the logic to parse the arguments.

  while (true) {
    auto ret = getopt_long(argc, argv, "ht:s:e:g:b:c:", longopts, nullptr);
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
      case 'b':
        try {
          batch_size = std::stol(optarg);
        } catch (...) {
          std::cerr << "--batch_size needs to be a valid number greather than 0"
              << " but got " << optarg << std::endl;
          print_help(argv[0], std::cout);
          return EXIT_FAILURE;
        }
        break;
      case 's':
        try {
          training_sessions = std::stol(optarg);
        } catch (...) {
          std::cerr << "--training_sessions needs to be a valid number greather than 0"
              << " but got " << optarg << std::endl;
          print_help(argv[0], std::cout);
          return EXIT_FAILURE;
        }
        break;
      case 'e':
        try {
          training_epochs = std::stol(optarg);
        } catch (...) {
          std::cerr << "--training_epochs needs to be a valid number greather than 0"
              << " but got " << optarg << std::endl;
          print_help(argv[0], std::cout);
          return EXIT_FAILURE;
        }
        break;
      case 'g':
        try {
          tournament_games = std::stol(optarg);
        } catch (...) {
          std::cerr << "--tournament_games needs to be a valid number greather than 0"
              << " but got " << optarg << std::endl;
          print_help(argv[0], std::cout);
          return EXIT_FAILURE;
        }
        break;
      case 'c':
        try {
          checkpoint_steps = std::stol(optarg);
        } catch (...) {
          std::cerr << "--checkpoint_steps needs to be a valid number greather than 0"
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
      .set_training_sessions(training_sessions)
      .set_training_games(training_games)
      .set_training_epochs(training_epochs)
      .set_tournament_games(tournament_games)
      .set_checkpoint_steps(checkpoint_steps)
      .set_batch_size(batch_size)
      .build()
      .train();
  } catch (std::exception& err) {
    std::cerr << err.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
