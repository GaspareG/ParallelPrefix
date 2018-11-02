#include <string>
#include <thread>
#include <algorithm>
#include <vector>
#include <numeric>
#include <utility>
#include <map>
#include <random>
#include "utils/clock.hpp"
#include "utils/cxxopts.hpp"

// Sequential implementation
#include "seq_stl.hpp"

// Block algorithm implementations
#include "block_stl.hpp"
#include "block_omp.hpp"
#include "block_ff.hpp"

#ifdef cilk_for
  #include "block_cilk.hpp"
#endif

// Circuit algorithm implemenation
#include "circuit_stl.hpp"
#include "circuit_omp.hpp"
#include "circuit_ff.hpp"

#ifdef cilk_for
  #include "circuit_cilk.hpp"
#endif

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

  // check experiments
  if(experiments == 0)
  {
    std::cout << "Error in number of experiments (check experiments option)" << std::endl;
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
  #ifdef cilk_for
  spm::block::parallelPrefixCilk<test_t, op> block_cilk(input, par_min);
  #endif
  spm::block::parallelPrefixFF<test_t, op> block_ff(input, par_min);
  spm::block::parallelPrefixOMP<test_t, op> block_omp(input, par_min);
  spm::block::parallelPrefixSTL<test_t, op> block_stl(input, par_min);

  // Circuit based
  #ifdef cilk_for
  spm::circuit::parallelPrefixCilk<test_t, op> circ_cilk(input, par_min);
  #endif
  spm::circuit::parallelPrefixFF<test_t, op> circ_ff(input, par_min);
  spm::circuit::parallelPrefixOMP<test_t, op> circ_omp(input, par_min);
  spm::circuit::parallelPrefixSTL<test_t, op> circ_stl(input, par_min);

  // Results storage
  std::map<std::tuple<std::string, unsigned int, unsigned int>, spm::timer::ms_t> res_times;

  // For each k
  for(unsigned int m = m_max; m >= m_min; --m)
  {
    std::cout << "m = " << m << " | n = " << (1<<m) << std::endl;

    (*input).resize(1<<m);
    output_seq.resize(1<<m);
    output_par.resize(1<<m);

    // Compute sequentially
    if(f_seq)
    {
      for(unsigned int i=0; i<experiments; i++)
      {
        output_seq.assign(output_seq.size(), 0);
        seq_stl.start(output_seq);
        res_times[{"seq_stl", m, 1}] += seq_stl.getLastTime(); 
      }
    }

    // For each parDeg
    for(unsigned int p=par_min; p<=par_max; p=(f_par_lin?(p+1):(p*2)))
    {
      std::cout << "\tp = " << p << std::endl;
      // For each experiment
      for(unsigned int i=0; i<experiments; i++)
      {

        // Test block_st
        if(f_stl_block)
        {
          output_par.assign(output_par.size(), 0);
          block_stl.start(output_par);
          res_times[{"block_stl", m, p}] += block_stl.getLastTime();
          if(f_check && output_seq != output_par)
          {
            std::cout << "ERROR IN BLOCK_STL!" << std::endl;
            return -1;
          }
        }

        // Test block_st
        if(f_ff_block)
        {
          output_par.assign(output_par.size(), 0);
          block_ff.start(output_par);
          res_times[{"block_ff", m, p}] += block_ff.getLastTime();
          if(f_check && output_seq != output_par)
          {
            std::cout << "ERROR IN BLOCK_FF!" << std::endl;
            return -1;
          }
        }

        // Test block_st
        if(f_omp_block)
        {
          output_par.assign(output_par.size(), 0);
          block_omp.start(output_par);
          res_times[{"block_omp", m, p}] += block_omp.getLastTime();
          if(f_check && output_seq != output_par)
          {
            std::cout << "ERROR IN BLOCK_STL!" << std::endl;
            return -1;
          }
        }

        // Test block_st
        #ifdef cilk_for
        if(f_cilk_block)
        {
          output_par.assign(output_par.size(), 0);
          block_stl.start(output_par);
          res_times[{"block_cilk", m, p}] += block_cilk.getLastTime();
          if(f_check && output_seq != output_par)
          {
            std::cout << "ERROR IN BLOCK_CILK!" << std::endl;
            return -1;
          }
        }
        #endif

        // Test circ_st
        if(f_stl_circ)
        {
          output_par.assign(output_par.size(), 0);
          circ_stl.start(output_par);
          res_times[{"circ_stl", m, p}] += circ_stl.getLastTime();
          if(f_check && output_seq != output_par)
          {
            std::cout << "ERROR IN CIRC_STL!" << std::endl;
            return -1;
          }
        }
        // Test circ_st
        if(f_ff_circ)
        {
          output_par.assign(output_par.size(), 0);
          circ_ff.start(output_par);
          res_times[{"circ_ff", m, p}] += circ_ff.getLastTime();
          if(f_check && output_seq != output_par)
          {
            std::cout << "ERROR IN CIRC_FF!" << std::endl;
            return -1;
          }
        }

        // Test circ_st
        if(f_omp_circ)
        {
          output_par.assign(output_par.size(), 0);
          circ_omp.start(output_par);
          res_times[{"circ_omp", m, p}] += circ_omp.getLastTime();
          if(f_check && output_seq != output_par)
          {
            std::cout << "ERROR IN CIRC_STL!" << std::endl;
            return -1;
          }
        }

        // Test circ_st
        #ifdef cilk_for
        if(f_cilk_circ)
        {
          output_par.assign(output_par.size(), 0);
          circ_stl.start(output_par);
          res_times[{"circ_cilk", m, p}] += circ_cilk.getLastTime();
          if(f_check && output_seq != output_par)
          {
            std::cout << "ERROR IN CIRC_CILK!" << std::endl;
            return -1;
          }
        }
        #endif
      }
    }
  }

  std::cout << "Results..." << std::endl;

  for(auto [k, t] : res_times)
  {
    auto [s, m, p] = k;
    t /= experiments;
    std::cout << s << " " << m << " " << p << " " << t << std::endl;
  }

 return 0;
}
