#include <cstdlib>
#include <getopt.h>
#include <fstream>
#include <ios>
#include <iostream>
#include <span>
#include <string>
#include <unistd.h>

#include "magics.h"
#include "pre_computed_magics.h"

using namespace blunder;

void
PrintHelp(std::string_view prog, std::ostream& os)
{
  os << "Usage: " << prog << " [options] ...\n"
     << "     -h|--help         Print this help message.\n"
     << "     -f|--file         The name of the file to save the magic numbers.\n"
     << "     -g|--generate     Generate the magic numbers from scratch. Mutually\n"
     << "                       exclusive of --precomputed.\n"
     << "     -p|--precomputed  Use pre-computed magic numbers. Mutually\n"
     << "                       exclusive of --generate.\n"
     << std::endl;
}

void
PrintMagics(std::span<const Magic> magics, std::string_view msg)
{
  std::cout << msg << '\n';
  for (const auto& magic : magics)
    std::cout << "0x" << std::hex << magic.magic << ",\n";
}

//template<size_t N>
void
PrintMagics(
    std::span<const Magic> bmagics,
    std::span<const Magic> rmagics)
{
  PrintMagics(bmagics, "The magic numbers for bishops are...");
  PrintMagics(rmagics, "The magic numbers for rooks are...");
}

std::pair<MagicAttacks, MagicAttacks>
GenerateMagics()
{
  // Handle bishops.
  auto magic_bishops = MagicAttacks::ComputeBishopMagics();
  if (not magic_bishops) {
    std::cerr << "Unable to compute magic attacks for bishops." << std::endl;
    std::exit(EXIT_FAILURE);
  }

  // Handle rooks.
  auto magic_rooks = MagicAttacks::ComputeRookMagics();
  if (not magic_rooks) {
    std::cerr << "Unable to compute magic attacks for rooks." << std::endl;
    std::exit(EXIT_FAILURE);
  }

  return std::make_pair(std::move(*magic_bishops), std::move(*magic_rooks));
}

std::pair<MagicAttacks, MagicAttacks>
UsePrecomputedMagics()
{
  // Handle bishops.
  auto magic_bishops =
    MagicAttacks::InitFromBishopMagics(kBishopMagics);
  if (not magic_bishops) {
    std::cerr << "Unable to use precomputed magics for bishops." << std::endl;
    std::exit(EXIT_FAILURE);
  }

  // Handle rooks.
  auto magic_rooks = MagicAttacks::InitFromRookMagics(kRookMagics);
  if (not magic_rooks) {
    std::cerr << "Unable to use precomputed magics for rooks." << std::endl;
    std::exit(EXIT_FAILURE);
  }

  return std::make_pair(std::move(*magic_bishops), std::move(*magic_rooks));
}

void
WriteMagics(
    const std::string& fname,
    std::span<const Magic> bmagics,
    std::span<const Magic> rmagics)
{
  if (fname.empty()) return;

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
    std::exit(EXIT_FAILURE);
  }
}

int
main(int argc, char** argv)
{
  std::string fname;
  bool generated = false;
  bool precomputed = false;

  struct option longopts[] = {
    {"help", no_argument, nullptr, 'h'},
    {"file", required_argument, nullptr, 'f'},
    {"generate", no_argument, nullptr, 'g'},
    {"precomputed", no_argument, nullptr, 'p'},
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
      case 'g':
        generated = true;
        break;
      case 'p':
        precomputed = true;
        break;
      default:
        std::cerr << "Received unknown command line option.\n";
        PrintHelp(argv[0], std::cerr);
        return EXIT_FAILURE;
    }
  }

  if (generated and precomputed) {
    std::cerr << "Either --generate or --precomputed should be selected.\n";
    PrintHelp(argv[0], std::cerr);
    return EXIT_FAILURE;
  } else if (not generated and not precomputed) {
    std::cerr << "One of --generate or --precomputed should be selected.\n";
    PrintHelp(argv[0], std::cerr);
    return EXIT_FAILURE;
  }

  auto [magic_bishops, magic_rooks] = generated
    ? GenerateMagics()
    : UsePrecomputedMagics();

  auto bmagics = magic_bishops.GetMagics();
  auto rmagics = magic_rooks.GetMagics();

  PrintMagics(bmagics, rmagics);
  WriteMagics(fname, bmagics, rmagics);

  return EXIT_SUCCESS;
}
