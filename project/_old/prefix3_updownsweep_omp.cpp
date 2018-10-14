#include <iostream>
#include <vector>
#include <chrono>
#include <omp.h>

std::vector< std::vector<int> > upSweep(std::vector<int>& V, int Nthread)
{
  size_t N = V.size();
  std::vector< std::vector<int> > Ad;
  Ad.push_back(V);
  for(size_t d=1; (1<<(d-1)) <= N; d++)
  {
    Ad.push_back( std::vector<int>( (N/(1<<(d))) ) );
    #pragma omp parallel for
    for(size_t i=0; i < (N/(1<<(d))); i++ )
      Ad[d][i] = Ad[d-1][2*i] + Ad[d-1][2*i+1];
  }
  return Ad;
}

void downSweep(std::vector<int>& V, std::vector< std::vector<int> >& Ad, int Nthread)
{
  size_t N = V.size();

  auto lCopy = [&]{ };
  auto lAdd = [&]{ };

  for(size_t dc = 1; dc <= Ad.size(); dc++)
  {
    size_t d = Ad.size()-dc;
    size_t limit = (N/(1<<(d)));
  
    
    #pragma omp parallel for
    for(size_t i=1; i < limit; i++)
    {
      if( i % 2 != 0 )
        Ad[d][i] = Ad[d+1][i/2];
      else
        Ad[d][i] += Ad[d+1][(i/2)-1];
    }
  }
  V = Ad[0];
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

  int Nthread = 1;
  if( argc > 2 )
    Nthread = atoi(argv[2]);

  // Read input
  int tmp;
  for(int i=0; i<N; i++)
  {
    std::cin >> tmp;
    V[i] = tmp;
  }

  // Calc prefix array
  auto start   = std::chrono::high_resolution_clock::now();
  auto Ad = upSweep(V, Nthread);
  downSweep(V, Ad, Nthread);
  auto elapsed = std::chrono::high_resolution_clock::now() - start;
  auto msec    = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();

  std::cerr << msec << std::endl;

  // Write output
  for(int i : V)
    std::cout << i << " ";
  std::cout << std::endl;

  return 0;
}
