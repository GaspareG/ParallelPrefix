#include <vector>
#include <functional>


namespace spm {

  class prefix<class T, int D> {

     public:

        prefix(std::vector<T> V, std::function<T(T,T)> F) {

        }

        virtual void start();


    protected:
      std::vector<T> v;
      std::function
  };

}
