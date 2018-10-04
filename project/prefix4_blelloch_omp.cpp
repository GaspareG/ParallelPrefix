#include <iostream>
#include <vector>
#include <chrono>
#include <omp.h>

void upSweep(std::vector<int>& V)
{
  size_t N = V.size();
  for(size_t d=0; (1<<(d-1)) <= N; d++)
  {
    #pragma omp parallel for
	  for(size_t k=0; k<N; k += (1<<(d+1)))
      V[k + (1<<(d+1)) - 1] += V[k + (1<<d) - 1];

  }
}


void downSweep(std::vector<int>& V)
{
  size_t N = V.size();

  V[N-1] = 0;

  size_t logn = 0;
  for(logn=0; (1<<(logn-1)) <= N; logn++);

  for(size_t dc=1; dc <= logn; dc++)
  {
    size_t d = logn-dc;
    #pragma omp parallel for
    for(size_t k=0; k<N; k += (1<<(d+1)) )
    {
      int t = V[k + (1<<d) - 1];
      V[k + (1<<d)     - 1] = V[k + (1<<(d+1)) - 1];
      V[k + (1<<(d+1)) - 1] += t;
    }
  }

}

int main(int argc, char **argv)
{
  if( argc < 1 )
  {
    std::cout << "Usage: " << argv[0] << " Nsize (Nthread)" << std::endl;
    exit(1);
  }

  int N = atoi(argv[1]);
  std::vector<int> V(N);

  if( argc > 2 )
  {
    int Nthread = atoi(argv[2]);
    omp_set_dynamic(0);
    omp_set_num_threads(Nthread);
  }

  // Read input
  int tmp;
  for(int i=0; i<N; i++)
  {
    std::cin >> tmp;
    V[i] = tmp;
  }

  //Calc prefix array
  auto start   = std::chrono::high_resolution_clock::now();
  V.push_back( V[V.size()-1] );

  upSweep(V);
  downSweep(V);

  auto elapsed = std::chrono::high_resolution_clock::now() - start;
  auto msec    = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();

  std::cerr << msec << std::endl;

  // Write output
  for(int i : V)
    std::cout << i << " ";
  std::cout << std::endl;

  return 0;
}
