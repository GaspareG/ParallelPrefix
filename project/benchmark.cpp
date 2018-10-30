#include <string>
#include "utils/clock.hpp"
#include "utils/cxxopts.hpp"

#include "seq_stl.hpp"
#include "block_stl.hpp"
#include "block_omp.hpp"
#include "block_ff.hpp"
// #include "block_cilk.hpp"

int main(int argc, char**argv)
{

  // Options default value
  bool help_f = false;
  bool verbose_f  = false;

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
    (    "v,verbose", "Verbose log",                               cxxopts::value(verbose_f));
 /*   (      "i,input", "Input file name (default: stdin)",          cxxopts::value(input_s))
    (     "o,output", "Output file name (default: stdout)",        cxxopts::value(output_s))
    // Algorithms to benchmark
    (   "sequential", "Test STL sequential implementation ",       cxxopts::value(sequential_f))
    (    "stl-block", "Test STL block implementation ",            cxxopts::value(stl_block_f))
    (     "stl-circ", "Test STL circuit implementation ",          cxxopts::value(stl_horn_f))
    (     "ff-block", "Test FF block implementation ",             cxxopts::value(ff_block_f))
    (      "ff-circ", "Test FF circuit implementation ",           cxxopts::value(ff_horn_f))
    (    "omp-block", "Test OMP block implementation ",            cxxopts::value(omp_block_f))
    (     "omp-circ", "Test OMP circuit implementation ",          cxxopts::value(omp_horn_f))
    (   "cilk-block", "Test CILK block implementation ",           cxxopts::value(omp_block_f))
    (    "cilk-circ", "Test CILK circuit implementation ",         cxxopts::value(omp_horn_f))
    (    "all-block", "Test all block implementations",            cxxopts::value(all_f))
    (  "all-circuit", "Test all circuit implementations",          cxxopts::value(all_f))
    (          "all", "Test all implementations",                  cxxopts::value(all_f))
    // Experiments parameters
    ("e,experiments", "Number of experiments to run (default: 1)", cxxopts::value(experiments))
    (   "p,parallel", "Parallelism degree (default: max)",         cxxopts::value(parallel))
    // Algorithms parameters
    (            "min-k", "Min power of size of the vector (default: 20 )",        cxxopts::value(N))
*/
  auto result = options.parse(argc, argv);

  return 0;
}
