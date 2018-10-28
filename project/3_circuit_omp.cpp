#include <bits/stdc++.h>

int op(int a, int b){ return a+b; }

int main()
{

  int m = 8;
  int n = 1<<m;

  std::vector<int> v(n);
  std::vector<int> v_seq(n);
  std::vector<int> v_par(n);

  for(int i=0; i<n; i++) v[i] = 1;

  std::partial_sum(std::begin(v), std::end(v), std::begin(v_seq), op);


  // Phase 1
  for(int t=1; t<=m; t++)
  {
    int kl = (1<<(m-t));
    #pragma omp parallel for schedule(static)
    for(int k=1; k<=kl; k++)
    {
        int l = (k*(1<<t))-(1<<(t-1))-1;
        int r = (k*(1<<t))-1;
        v[r] = op(v[l], v[r]);
    }
  }


  // Phase 2
  for(int t=1; t<m; t++)
  {
    int kl = (1<<t)-1;
    #pragma omp parallel for schedule(static)
    for(int k=1; k<=kl; k++)
    {
        int l = (k*(1<<(m-t)))-1;
        int r = (k*(1<<(m-t)))+(1<<(m-t-1))-1;
        v[r] = op(v[l], v[r]);
    }
  }

  std::cout << std::endl;
  for(int x : v) std::cout << x << " ";
  std::cout << std::endl;
  for(int x : v_seq) std::cout << x << " ";
  std::cout << std::endl;

  return 0;
}
