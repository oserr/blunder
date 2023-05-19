#include <cstdlib>
#include <ios>
#include <iostream>

#include "magics.h"

using namespace blunder;

int
main()
{
  auto magic_attacks = MagicAttacks::ComputeBishopMagics();
  if (not magic_attacks) {
    std::cerr << "Unable to compute magic attacks for bishops." << std::endl;
    return EXIT_FAILURE;
  }

  auto magics = magic_attacks->GetMagics();
  std::cout << "The magic numbers for bishops are...\n";
  for (const auto& magic : magics)
    std::cout << std::hex <<  magic.magic << ",\n";

  return EXIT_SUCCESS;
}
