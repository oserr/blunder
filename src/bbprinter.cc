#include "bitboard_io.h"

#include <algorithm>
#include <getopt.h>
#include <functional>
#include <iostream>
#include <string>
#include <string_view>
#include <unistd.h>

#include "bitboard.h"

using namespace blunder;

void
PrintHelp(std::string_view prog, std::ostream& os)
{
  os << "Usage: " << prog << " [options] ...\n"
     << "     -h|--help      Print this help message.\n"
     << "     -s|--square    The square to print the mask for.\n"
     << "     -d|--diag      If set, prints the diagonal mask.\n"
     << "     -f|--file      If set, prints the rank and file mask.\n"
     << "     -b|--bmask     If set, prints the bishop mask for magic numbers.\n"
     << "     -r|--rmask     If set, prints the rook mask for magic numbers.\n"
     << std::endl;
}

int
main(int argc, char** argv)
{
  int square = -1;
  bool print_dmask = false;
  bool print_fmask = false;
  bool print_bmask = false;
  bool print_rmask = false;

  struct option longopts[] = {
    {"help", no_argument, nullptr, 'h'},
    {"diag", no_argument, nullptr, 'd'},
    {"file", no_argument, nullptr, 'f'},
    {"square", required_argument, nullptr, 's'},
    {"bmask", no_argument, nullptr, 'b'},
    {"rmask", no_argument, nullptr, 'r'},
    {0, 0, 0, 0},
  };

  while (true) {
    auto ret = getopt_long(argc, argv, "hdfs:", longopts, nullptr);

    if (ret == -1) break;

    switch (ret) {
      case '?':
        std::cerr << "Received unknown command line option.\n";
        PrintHelp(argv[0], std::cerr);
        return EXIT_FAILURE;
      case 'h':
        PrintHelp(argv[0], std::cout);
        return EXIT_SUCCESS;
      case 'd':
        print_dmask = true;
        break;
      case 'f':
        print_fmask = true;
        break;
      case 'b':
        print_bmask = true;
        break;
      case 'r':
        print_rmask = true;
        break;
      case 's':
        try {
          square = std::stoi(optarg);
        } catch (...) {
          std::cerr << "Argument for -s/--square is not valid." << std::endl;
          return EXIT_FAILURE;
        }
        if (square < 0 or square > 63) {
          std::cerr << "-s/--square should be in range [0,63]." << std::endl;
          return EXIT_FAILURE;
        }
        break;
      default:
        break;
    }
  }

  const bool flags[4] = {
    print_dmask,
    print_fmask,
    print_bmask,
    print_rmask
  };

  if (square == -1) {
    std::cerr << "Need to specify a square.\n";
    PrintHelp(argv[0], std::cerr);
    return EXIT_FAILURE;
  } else if (std::ranges::none_of(flags, std::identity())) {
    std::cerr << "Need to specify a mask for the square.\n";
    PrintHelp(argv[0], std::cerr);
    return EXIT_FAILURE;
  }

  auto diag_mask = kDiagMask[square];
  auto file_mask = kFileRankMask[square];

  if (print_dmask and print_fmask) {
    std::cout << "Diagonal, rank, and file mask...\n";
    std::cout << ToMailboxStr(diag_mask | file_mask) << std::endl;
  } else if (print_dmask) {
    std::cout << "Diagonal mask...\n";
    std::cout << ToMailboxStr(diag_mask) << std::endl;
  } else if (print_fmask) {
    std::cout << "Rank and file mask...\n";
    std::cout << ToMailboxStr(file_mask) << std::endl;
  }

  if (print_bmask) {
    std::cout << "Bishop mask for magic numbers...\n";
    std::cout << ToMailboxStr(GetBishopMask(square)) << std::endl;
  }
  if (print_rmask) {
    std::cout << "Rook mask for magic numbers...\n";
    std::cout << ToMailboxStr(GetRookMask(square)) << std::endl;
  }

  return EXIT_SUCCESS;
}
