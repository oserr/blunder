#include <cstdlib>
#include <getopt.h>
#include <iostream>
#include <memory>
#include <string_view>
#include <tuple>
#include <unistd.h>

#include "magic_attacks.h"
#include "pre_computed_magics.h"
#include "timer.h"

#include "par.h"

using namespace blunder;

void
print_help(std::string_view prog, std::ostream& os)
{
  os << "Usage: " << prog << " [options] ...\n"
     << "   -h|--help  Print this help message.\n"
     << "   -r|--runs  The number of runs to use for computing magics from scratch\n"
     << "              and from pre-computed magics.\n"
     << std::endl;
}

std::tuple<Timer, Timer>
run_bench(unsigned nruns)
{
  Timer compute_timer;
  for (unsigned i = 0; i < nruns; ++i) {
    compute_timer.start();
    auto bmagics = compute_bmagics();
    auto rmagics = compute_rmagics();
    compute_timer.end();
  }

  Timer from_timer;
  for (unsigned i = 0; i < nruns; ++i) {
    from_timer.start();
    auto bmagics = from_bmagics(kBishopMagics);
    auto rmagics = from_rmagics(kRookMagics);
    from_timer.end();
  }

  return std::make_tuple(compute_timer, from_timer);
}

std::tuple<Timer, Timer>
run_bench(std::unique_ptr<const MagicComputer> magic_computer, unsigned nruns)
{
  Timer compute_timer;
  for (unsigned i = 0; i < nruns; ++i) {
    compute_timer.start();
    auto bmagics = magic_computer->compute_bmagics();
    auto rmagics = magic_computer->compute_rmagics();
    compute_timer.end();
  }

  Timer from_timer;
  for (unsigned i = 0; i < nruns; ++i) {
    from_timer.start();
    auto bmagics = magic_computer->from_bmagics(kBishopMagics);
    auto rmagics = magic_computer->from_rmagics(kRookMagics);
    from_timer.end();
  }

  return std::make_tuple(compute_timer, from_timer);
}

int
main(int argc, char** argv)
{
  struct option longopts[] = {
    {"help", no_argument, nullptr, 'h'},
    {"runs", required_argument, nullptr, 'r'},
    {0, 0, 0, 0},
  };

  unsigned runs = 100;

  while (true) {
    auto ret = getopt_long(argc, argv, "hr:", longopts, nullptr);
    if (ret == -1) break;
    switch (ret) {
      case 'h':
        print_help(argv[0], std::cout);
        return EXIT_SUCCESS;
      case 'r':
        try {
          runs = std::stol(optarg);
        } catch (...) {
          std::cerr << "--runs needs to be a valid, but got "
                    << optarg << std::endl;
          print_help(argv[0], std::cout);
          return EXIT_FAILURE;
        }
        break;
      default:
        std::cerr << "Received unknown command line option.\n";
        print_help(argv[0], std::cout);
        return EXIT_FAILURE;
    }
  }

  std::cout << "Running bench for ParMagicComputer with "
            << runs << " runs!" << std::endl;

  auto [par_compute_timer, par_from_timer] = run_bench(
      std::make_unique<ParMagicComputer>(
        std::make_shared<par::WorkQ>(par::WorkQ::with_all_threads())),
      runs);

  std::cout << "Finished bench for ParMagicComputer. "
            << "Now running bench for single theaded functions with "
            << runs << " runs!" << std::endl;

  auto [compute_timer, from_timer] = run_bench(runs);

  std::cout << "Bench stats with " << runs << " runs:\n"
            << "\tsingle threaded\n"
            << "\t\tfrom scratch:\n"
            << "\t\t\ttotal: " << compute_timer.total_seconds() << " s\n"
            << "\t\t\tavg:   " << compute_timer.avg_millis() << " ms\n"
            << "\t\tfrom pre-computed:\n"
            << "\t\t\ttotal: " << from_timer.total_seconds() << " s\n"
            << "\t\t\tavg:   " << from_timer.avg_millis() << " ms\n"
            << "\tmulti-threaded\n"
            << "\t\tfrom scratch:\n"
            << "\t\t\ttotal: " << par_compute_timer.total_seconds() << " s\n"
            << "\t\t\tavg:   " << par_compute_timer.avg_millis() << " ms\n"
            << "\t\tfrom pre-computed:\n"
            << "\t\t\ttotal: " << par_from_timer.total_seconds() << " s\n"
            << "\t\t\tavg:   " << par_from_timer.avg_millis() << " ms\n"
            << "\tspeedup:\n"
            << "\t\tfrom sratch: "
            << compute_timer.avg_millis() / par_compute_timer.avg_millis()
            << '\n'
            << "\t\tfrom pre-computed: "
            << from_timer.avg_millis() / par_from_timer.avg_millis()
            << '\n'
            << std::endl;

  return EXIT_SUCCESS;
}
