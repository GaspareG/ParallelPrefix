#include <string>
#include "clock.hpp"
#include "cxxopts.hpp"

int main(int argc, char**argv)
{

  // Options default value
  bool help = false;
  bool verbose = false;

  std::string input_s = "";
  std::string output_s = "";

  unsigned int experiments = 1;
  unsigned int parallel = 0;
  unsigned int N = 1048576;

  bool sequential_f = false;

  bool stl_block_f = false;
  bool stl_horn_f = false;

  bool ff_block_f = false;
  bool ff_horn_f = false;

  bool omp_block_f = false;
  bool omp_horn_f = false;

  bool all_f = false;

  // Parse arguments
  cxxopts::Options options("benchmark", "Benchmark of all prefix-sum implementations");
  options.add_options()
    // Help flag
    (       "h,help", "Print help",                                cxxopts::value(help_f))
    // Stream options
    (    "v,verbose", "Verbose log",                               cxxopts::value(verbose_f))
    (      "i,input", "Input file name (default: stdin)",          cxxopts::value(input_s))
    (     "o,output", "Output file name (default: stdout)",        cxxopts::value(output_s))
    // Algorithms to benchmark
    (   "sequential", "Test stl sequential implementation ",       cxxopts::value(sequential_f))
    (    "stl-block", "Test stl block implementation ",            cxxopts::value(stl_block_f))
    (     "stl-horn", "Test stl horn implementation ",             cxxopts::value(stl_horn_f))
    (     "ff-block", "Test ff block implementation ",             cxxopts::value(ff_block_f))
    (      "ff-horn", "Test ff horn implementation ",              cxxopts::value(ff_horn_f))
    (    "omp-block", "Test omp block implementation ",            cxxopts::value(omp_block_f))
    (     "omp-horn", "Test omp horn implementation ",             cxxopts::value(omp_horn_f))
    (          "all", "Test all implementations",                  cxxopts::value(all_f))
    // Experiments parameters
    ("e,experiments", "Number of experiments to run (default: 1)", cxxopts::value(experiments))
    (   "p,parallel", "Parallelism degree (default: max)",         cxxopts::value(parallel))
    // Algorithms parameters
    (            "n", "Size of the vector (default: 2^20)",        cxxopts::value(N))

  auto result = options.parse(argc, argv);

  return 0;
}
