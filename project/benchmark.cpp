#include <string>
#include <thread>
#include <algorithm>
#include <vector>
#include <numeric>
#include <utility>
#include <random>
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
#define M_MIN_DEFAULT 26
#define M_MAX_DEFAULT 30

#define M_MIN 1
#define M_MAX 30

#define MAX_V 1024

// vector type and lambda operation
using test_t = unsigned long long int;
auto op = [](test_t a, test_t b) -> test_t
{
  return a+b;
};

template <typename T>
std::ostream& operator<< (std::ostream& out, const std::vector<T>& v) {
  if ( !v.empty() ) {
    out << '[';
    std::copy (v.begin(), v.end(), std::ostream_iterator<T>(out, ", "));
    out << "\b\b]";
  }
  out << std::endl;
  return out;
}

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
  unsigned int seed = 42;
  bool f_par_lin = false;
  bool f_check = false;

  // Algorithms parameters
  unsigned int m_min = M_MIN_DEFAULT;
  unsigned int m_max = M_MAX_DEFAULT;

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
    (        "seed", "Seed for random number generator (default 42)",  cxxopts::value(seed))

    // Algorithms parameters
    (       "m-min", "Min log2 of vector size (default: 26)",          cxxopts::value(m_min))
    (       "m-max", "Max log2 of vector size (default: 30)",          cxxopts::value(m_max));

  // Parse arguments
  auto result = options.parse(argc, argv);

  // Optimal configurations
  if(f_complete)
  {
    f_all = true;
    f_check = true;
    experiments = 5;
    dataset = 1;
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
    return 1;
  }

  // Check vector size
  if(!(M_MIN <= m_min && m_min <= m_max && m_max <= M_MAX))
  {
    std::cout << "Error in vector size (check m_min and m_max options)" << std::endl;
    return 1;
  }

  // Check parallelism degree
  if(!(1 <= par_min && par_min <= par_max && par_max <= max_threads))
  {
    std::cout << "Error in parallelism degree (check par_min and par_max options)" << std::endl;
    return 1;
  }

  // Check dataset
  if(!(0 <= dataset && dataset <= 2))
  {
    std::cout << "Error in dataset creation (check dataset option)" << std::endl;
    return 1;
  }

  // Create dataset
  auto input = std::make_shared<std::vector<test_t>>(1<<m_max, 0);
  std::vector<test_t> output_seq(1<<m_max, 0);
  std::vector<test_t> output_par(1<<m_max, 0);

  std::cout << dataset << std::endl;
  if(dataset == 0)
  {
    std::mt19937 rng(seed);
    for(int i=0; i<(1<<m_max); i++)
      (*input)[i] = rng() % MAX_V;
  }
  else if(dataset == 1)
  {
    for(int i=0; i<(1<<m_max); i++)
      (*input)[i] = 1;
  }
  else
  {
    for(int i=0; i<(1<<m_max); i++)
      (*input)[i] = i;
  }

  // Create class for computation...

  // Sequential
  spm::sequential::sequentialPrefixSTL<test_t, op> seq_stl(input);

  // Block-based
  // spm::block::parallelPrefixCilk<test_t, op> block_cilk(input, par_min);
  spm::block::parallelPrefixFF<test_t, op> block_ff(input, par_min);
  spm::block::parallelPrefixOMP<test_t, op> block_omp(input, par_min);
  spm::block::parallelPrefixSTL<test_t, op> block_stl(input, par_min);

  // Circuit based
  // spm::circuit::parallelPrefixCilk<test_t, op> circuit_cilk(input, par_min);
  spm::circuit::parallelPrefixFF<test_t, op> circuit_ff(input, par_min);
  spm::circuit::parallelPrefixOMP<test_t, op> circuit_omp(input, par_min);
  spm::circuit::parallelPrefixSTL<test_t, op> circuit_stl(input, par_min);

  // TEST
  std::cout << "Sequential..." << std::endl;
  seq_stl.start(output_seq);
  std::cout << output_seq[output_seq.size()-1] << std::endl;
  //std::cout << output_seq;

  std::cout << "Parallel ff..." << std::endl;
  output_par.assign(output_par.size(), 0);
  block_ff.start(output_par);
  std::cout << "Check " << ((output_seq != output_par) ? "false": "true") << std::endl;
  std::cout << output_par[output_par.size()-1] << std::endl;
  //std::cout << output_par;

  std::cout << "Parallel omp..." << std::endl;
  output_par.assign(output_par.size(), 0);
  block_omp.start(output_par);
  std::cout << "Check " << ((output_seq != output_par) ? "false": "true") << std::endl;
  std::cout << output_par[output_par.size()-1] << std::endl;
  //std::cout << output_par;

  std::cout << "Parallel stl..." << std::endl;
  output_par.assign(output_par.size(), 0);
  block_stl.start(output_par);
  std::cout << "Check " << ((output_seq != output_par) ? "false": "true") << std::endl;
  std::cout << output_par[output_par.size()-1] << std::endl;
  //std::cout << output_par;

  std::cout << "Circuit ff..." << std::endl;
  output_par.assign(output_par.size(), 0);
  circuit_ff.start(output_par);
  std::cout << "Check " << ((output_seq != output_par) ? "false": "true") << std::endl;
  std::cout << output_par[output_par.size()-1] << std::endl;
  //std::cout << output_par;

  std::cout << "Circuit omp..." << std::endl;
  output_par.assign(output_par.size(), 0);
  circuit_omp.start(output_par);
  std::cout << "Check " << ((output_seq != output_par) ? "false": "true") << std::endl;
  std::cout << output_par[output_par.size()-1] << std::endl;
  //std::cout << output_par;

  std::cout << "Circuit stl..." << std::endl;
  output_par.assign(output_par.size(), 0);
  circuit_stl.start(output_par);
  std::cout << "Check " << ((output_seq != output_par) ? "false": "true") << std::endl;
  std::cout << output_par[output_par.size()-1] << std::endl;
  //std::cout << output_par;

  // For each k
  for(unsigned int m = m_max; m >= m_min; --m)
  {
    // For each parDeg
    for(unsigned int p=par_min; p<=par_max; p=(f_par_lin?(p+1):(p*2)))
    {
      // TEST
    }
  }

  return 0;
}
