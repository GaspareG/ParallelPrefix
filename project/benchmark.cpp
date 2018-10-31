#include <string>
#include <thread>
#include <algorithm>
#include <vector>
#include <numeric>
#include <utility>
#include "utils/clock.hpp"
#include "utils/cxxopts.hpp"

// Sequential implementation
#include "seq_stl.hpp"

// Block algorithm implementations
#include "block_stl.hpp"
#include "block_omp.hpp"
#include "block_ff.hpp"
// #include "block_cilk.hpp"

// Circuit algorithm implemenation
#include "circuit_stl.hpp"
#include "circuit_omp.hpp"
#include "circuit_ff.hpp"
// #include "circuit_cilk.hpp"

// Vector size define
#define K_MIN_DEFAULT 26
#define K_MAX_DEFAULT 30

#define K_MIN 10
#define K_MAX 30

// vector type and lambda operation
using test_t = unsigned long long int;
auto op = [](const test_t a, const test_t b)
{
  return a+b;
};

// Benchmark
int main(int argc, char**argv)
{

  // Max available parallelism degree
  const unsigned int max_threads = std::thread::hardware_concurrency();

  // Options default value
  bool f_help = false;
  bool f_verbose = false;
  bool f_complete = false;

  // Algorithm flags
  bool f_seq        = false;
  bool f_stl_block  = false;
  bool f_stl_circ   = false;
  bool f_ff_pipe    = false;
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
  unsigned int par_max = max_threads;
  unsigned int dataset = 0;
  bool f_par_lin = false;
  bool f_check = false;

  // Algorithms parameters
  unsigned int k_min = K_MIN_DEFAULT;
  unsigned int k_max = K_MAX_DEFAULT;

  // Parse arguments
  cxxopts::Options options("benchmark", "Benchmark of all parallel-prefix implementations");
  options.add_options()
    // Options
    (      "h,help", "Print help",                                     cxxopts::value(f_help))
    (   "v,verbose", "Verbose log to std::cerr (defaut: false)",       cxxopts::value(f_verbose))
    (  "c,complete", "Execute smart benchmark (default: false)",       cxxopts::value(f_complete))

    // Algorithms to benchmark
    (     "stl-seq", "Test STL sequential implementation ",            cxxopts::value(f_seq))
    (   "stl-block", "Test STL block implementation ",                 cxxopts::value(f_stl_block))
    (    "stl-circ", "Test STL circuit implementation ",               cxxopts::value(f_stl_circ))
    (    "ff-block", "Test FF block implementation ",                  cxxopts::value(f_ff_block))
    (     "ff-pipe", "Test FF pipeline block implementation ",         cxxopts::value(f_ff_pipe))
    (     "ff-circ", "Test FF circuit implementation ",                cxxopts::value(f_ff_circ))
    (   "omp-block", "Test OMP block implementation ",                 cxxopts::value(f_omp_block))
    (    "omp-circ", "Test OMP circuit implementation ",               cxxopts::value(f_omp_circ))
    (  "cilk-block", "Test CILK block implementation ",                cxxopts::value(f_cilk_block))
    (   "cilk-circ", "Test CILK circuit implementation ",              cxxopts::value(f_cilk_circ))
    (   "all-block", "Test all block implementations",                 cxxopts::value(f_all_block))
    ( "all-circuit", "Test all circuit implementations",               cxxopts::value(f_all_circ))
    (         "all", "Test all implementations",                       cxxopts::value(f_all))

    // Experiments parameters
    (         "exp", "N° of experiments to run (default: 1)",          cxxopts::value(experiments))
    (     "par-min", "Minimum parallelism degree (default: 1)",        cxxopts::value(par_min))
    (     "par-max", "Maximum parallelism degree (default: max)",      cxxopts::value(par_max))
    (     "par-lin", "Linear step in par degree (default: false)",     cxxopts::value(f_par_lin))
    (       "check", "Check for errors (default: false)",              cxxopts::value(f_check))
    (     "dataset", "Input dataset (default 0: rnd, 1: 1s, 2: incr)", cxxopts::value(dataset))

    // Algorithms parameters
    (       "k-min", "Min Log2 of vector size (default: K_MIN_DEFAULT)", cxxopts::value(k_min))
    (       "k-max", "Max Log2 of vector size (default: K_MAX_DEFAULT)", cxxopts::value(k_max))
  ;

  // Parse arguments
  auto result = options.parse(argc, argv);

  // Optimal configurations
  if(f_complete)
  {
    f_all = true;
    f_check = true;
    experiments = 5;
    k_min = 20;
  }

  // Set experiments flag
  if(f_all)
  {
    f_seq = true;
    f_all_block = true;
    f_all_circ = true;
  }

  if(f_all_circ)
  {
    f_stl_circ   = true;
    f_ff_circ    = true;
    f_omp_circ   = true;
    f_cilk_circ  = true;
  }

  if(f_all_block)
  {
    f_stl_block  = true;
    f_ff_block   = true;
    f_omp_block  = true;
    f_cilk_block = true;
  }

  if(f_check)
  {
    f_seq = true;
  }

  bool f_experiments = f_seq || f_stl_block || f_stl_circ;
  f_experiments |= f_ff_block || f_ff_pipe || f_ff_circ;
  f_experiments |= f_omp_block || f_omp_circ;
  f_experiments |= f_cilk_block || f_cilk_circ;

  // Request help or not choosing any algorithm
  if(f_help || !f_experiments)
  {
    std::cout << options.help() << std::endl;
    return 0;
  }

  if(!(K_MIN <= k_min && k_min <= k_max && k_max <= K_MAX))
  {
    std::cout << "Error in vector size (check k_min and k_max options)" << std::endl;
    return 0;
  }

  if(!(1 <= par_min && par_min <= par_max && par_max <= max_threads))
  {
    std::cout << "Error in parallelism degree (check par_min and par_max options)" << std::endl;
    return 0;
  }

  // Create dataset
  std::shared_ptr<std::vector<test_t>> input_data = std::make_shared<std::vector<test_t>>();

  // For each k
  for(unsigned int k = k_max; k >= k_min; --k)
  {
    // For each parDeg
    for(unsigned int p=par_min; p<=par_max; p=(f_par_lin?(p+1):(p*2)))
    {
      // TEST
    }
  }

  return 0;
}
