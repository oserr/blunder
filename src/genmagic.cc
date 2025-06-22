#include <cstdlib>
#include <getopt.h>
#include <fstream>
#include <ios>
#include <iostream>
#include <span>
#include <string>
#include <unistd.h>

#include "magic_attacks.h"
#include "pre_computed_magics.h"

using namespace blunder;

void
print_help(std::string_view prog, std::ostream& os)
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
print_magics(std::span<const Magic> magics, std::string_view msg)
{
  std::cout << msg << '\n';
  for (const auto& magic : magics)
    std::cout << "0x" << std::hex << magic.magic << ",\n";
}

//template<size_t N>
void
print_magics(
    std::span<const Magic> bmagics,
    std::span<const Magic> rmagics)
{
  print_magics(bmagics, "The magic numbers for bishops are...");
  print_magics(rmagics, "The magic numbers for rooks are...");
}

std::pair<MagicAttacks, MagicAttacks>
gen_magics()
{
  auto magic_bishops = compute_bmagics();
  auto magic_rooks = compute_rmagics();
  return std::make_pair(std::move(magic_bishops), std::move(magic_rooks));
}

std::pair<MagicAttacks, MagicAttacks>
init_magics()
{
  auto magic_bishops = from_bmagics(kBishopMagics);
  auto magic_rooks = from_rmagics(kRookMagics);
  return std::make_pair(std::move(magic_bishops), std::move(magic_rooks));
}

void
write_magics(
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
    auto ret = getopt_long(argc, argv, "hgpf:", longopts, nullptr);
    if (ret == -1) break;
    switch (ret) {
      case 'h':
        print_help(argv[0], std::cout);
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
        print_help(argv[0], std::cerr);
        return EXIT_FAILURE;
    }
  }

  if (generated and precomputed) {
    std::cerr << "Either --generate or --precomputed should be selected.\n";
    print_help(argv[0], std::cerr);
    return EXIT_FAILURE;
  } else if (not generated and not precomputed) {
    std::cerr << "One of --generate or --precomputed should be selected.\n";
    print_help(argv[0], std::cerr);
    return EXIT_FAILURE;
  }

  try {
    auto [magic_bishops, magic_rooks] = generated
      ? gen_magics()
      : init_magics();

    auto bmagics = magic_bishops.get_magics();
    auto rmagics = magic_rooks.get_magics();

    print_magics(bmagics, rmagics);
    write_magics(fname, bmagics, rmagics);
  } catch (std::exception& err) {
    std::cerr << "Unable to compute the magics: " << err.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
