#include <assert.h>
#include "../cgl/cgl.hpp"
#include "../cgl/cgl.cpp"

using namespace std;

int main(int argc, char* argv[]) {
  /*
    assert(argc == 3); // dim, max-iter, density
    unsigned int maxiter = (unsigned int)atoi(argv[1]);
    float density = (float)atof(argv[2]);
    Cgl<10> c(maxiter, density);
    c.prepareGrid();
    c.printGrid();
    c.startCgl();
  */
    bitset<16> init;
    init.set(0);
    init.set(2);
    init.set(3);
    init.set(6);
    init.set(10);
    init.set(12);
    init.set(14);
    init.set(15);
    Cgl<4> c(init,200);
    c.startCgl();
}
