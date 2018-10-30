#include <string>
#include <thread>
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
  bool f_help = false;
  bool f_verbose = false;
  bool f_complete = false;

  // Algorithm flags
  bool f_seq        = false;
  bool f_stl_block  = false;
  bool f_stl_circ   = false;
  bool f_ff_block   = false;
  bool f_ff_circ    = false;
  bool f_omp_block  = false;
  bool f_omp_circ   = false;
  bool f_cilk_block = false;
  bool f_cilk_circ  = false;

  bool f_all_block  = false;
  bool f_all_circ   = false;
  bool f_all        = false;

  // Experiments parameters
  unsigned int experiments = 1;
  unsigned int par_min = 1;
  unsigned int par_max = std::thread::hardware_concurrency();
  bool f_par_lin = false;
  bool f_check = false;

  // Algorithms parameters
  unsigned int k_min = 26;
  unsigned int k_max = 30;

  // Parse arguments
  cxxopts::Options options("benchmark", "Benchmark of all parallel-prefix implementations");
  options.add_options()
    // Options
    (      "h,help", "Print help",                                 cxxopts::value(f_help))
    (   "v,verbose", "Verbose log (to std::cerr)",                 cxxopts::value(f_verbose))
    (  "c,complete", "Execute smart benchmark (default: false)",   cxxopts::value(f_complete))

    // Algorithms to benchmark
    (  "sequential", "Test STL sequential implementation ",        cxxopts::value(f_seq))
    (   "stl-block", "Test STL block implementation ",             cxxopts::value(f_stl_block))
    (    "stl-circ", "Test STL circuit implementation ",           cxxopts::value(f_stl_circ))
    (    "ff-block", "Test FF block implementation ",              cxxopts::value(f_ff_block))
    (     "ff-circ", "Test FF circuit implementation ",            cxxopts::value(f_ff_circ))
    (   "omp-block", "Test OMP block implementation ",             cxxopts::value(f_omp_block))
    (    "omp-circ", "Test OMP circuit implementation ",           cxxopts::value(f_omp_circ))
    (  "cilk-block", "Test CILK block implementation ",            cxxopts::value(f_cilk_block))
    (   "cilk-circ", "Test CILK circuit implementation ",          cxxopts::value(f_cilk_circ))
    (   "all-block", "Test all block implementations",             cxxopts::value(f_all_block))
    ( "all-circuit", "Test all circuit implementations",           cxxopts::value(f_all_circ))
    (         "all", "Test all implementations",                   cxxopts::value(f_all))

    // Experiments parameters
    (       "e,exp", "N° of experiments to run (default: 1)",      cxxopts::value(experiments))
    (     "par-min", "Minimum parallelism degree (default: 1)",    cxxopts::value(par_min))
    (     "par-max", "Maximum parallelism degree (default: max)",  cxxopts::value(par_max))
    (     "par-lin", "Linear step in par degree (default: false)", cxxopts::value(f_par_lin))
    (       "check", "Check for errors (default: false)",          cxxopts::value(f_check))

    // Algorithms parameters
    (       "k-min", "Min Log2 of vector size (default: 26)",        cxxopts::value(k_min))
    (       "k-max", "Max Log2 of vector size (default: 30)",        cxxopts::value(k_max))


  ;
  auto result = options.parse(argc, argv);

  if(f_help)
  {
    std::cout << options.help() << std::endl;
    return 0;
  }

  return 0;
}
