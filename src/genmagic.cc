#include <cstdlib>
#include <getopt.h>
#include <fstream>
#include <ios>
#include <iostream>
#include <string>
#include <unistd.h>

#include "magics.h"

using namespace blunder;

void
PrintHelp(std::string_view prog, std::ostream& os)
{
  os << "Usage: " << prog << " [options] ...\n"
     << "     -h|--help      Print this help message.\n"
     << "     -f|--file      The name of the file to save the magic numbers.\n"
     << std::endl;
}

int
main(int argc, char** argv)
{
  std::string fname;

  struct option longopts[] = {
    {"help", no_argument, nullptr, 'h'},
    {"file", required_argument, nullptr, 'f'},
    {0, 0, 0, 0},
  };

  while (true) {
    auto ret = getopt_long(argc, argv, "hf:", longopts, nullptr);
    if (ret == -1) break;
    switch (ret) {
      case 'h':
        PrintHelp(argv[0], std::cout);
        return EXIT_SUCCESS;
      case 'f':
        fname = optarg;
        break;
      default:
        std::cerr << "Received unknown command line option.\n";
        PrintHelp(argv[0], std::cerr);
        return EXIT_FAILURE;
    }
  }

  // Handle bishops.
  auto magic_bishops = MagicAttacks::ComputeBishopMagics();
  if (not magic_bishops) {
    std::cerr << "Unable to compute magic attacks for bishops." << std::endl;
    return EXIT_FAILURE;
  }

  auto bmagics = magic_bishops->GetMagics();
  std::cout << "The magic numbers for bishops are...\n";
  for (const auto& magic : bmagics)
    std::cout << "0x" << std::hex <<  magic.magic << ",\n";

  // Handle rooks.
  auto magic_rooks = MagicAttacks::ComputeRookMagics();
  if (not magic_rooks) {
    std::cerr << "Unable to compute magic attacks for rooks." << std::endl;
    return EXIT_FAILURE;
  }

  auto rmagics = magic_rooks->GetMagics();
  std::cout << "The magic numbers for rooks are...\n";
  for (const auto& magic : rmagics)
    std::cout << "0x" << std::hex << magic.magic << ",\n";

  if (fname.empty()) return EXIT_SUCCESS;

  // Save magics to file.
  std::cout << "Writing magic numbers to file " << fname << std::endl;
  std::ofstream ofile(fname);

  ofile << "constexpr std::array<BitBoard, 64> kBishopMagics = {\n";
  for (const auto& magic : bmagics)
    ofile << "0x" << std::hex <<  magic.magic << ",\n";
  ofile << "};\n\n";

  ofile << "constexpr std::array<BitBoard, 64> kRookMagics = {\n";
  for (const auto& magic : rmagics)
    ofile << "0x" << std::hex <<  magic.magic << ",\n";
  ofile << "};\n";

  if (not ofile) {
    std::cerr << "Encountered error while trying to write magic numbers."
              << std::endl;
  }

  return EXIT_SUCCESS;
}
