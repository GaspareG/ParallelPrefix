#include <iostream>
#include <vector>
#include <chrono>
#include <omp.h>

void horn(std::vector<int>& V)
{

  size_t N = V.size();
  std::vector<int> Vold = V;
  std::vector<int> Vnew(N);
  for(size_t i=1; (1<<(i-1)) <= N; i++)
  {
    #pragma omp parallel for
    for(size_t k=0; k < N; k++)
    {
      if( k < (1<<(i-1)) )  Vnew[k] = Vold[k];
      else                  Vnew[k] = Vold[k] + Vold[k - (1<<(i-1))];
    }
    std::swap(Vold, Vnew);
  }
  V = Vold;
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

  // Calc prefix array

  auto start   = std::chrono::high_resolution_clock::now();
  horn(V);
  auto elapsed = std::chrono::high_resolution_clock::now() - start;
  auto msec    = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();

  std::cerr << msec << std::endl;
  // Write output
  for(int i : V)
    std::cout << i << " ";
  std::cout << std::endl;

  return 0;
}
