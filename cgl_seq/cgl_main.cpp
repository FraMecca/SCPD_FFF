#include <assert.h>
#include "cgl.hpp"
#include "cgl.cpp"

using namespace std;

int main(int argc, char* argv[]) {
    assert(argc == 3); // dim, max-iter, density
    unsigned int maxiter = (unsigned int)atoi(argv[1]);
    float density = (float)atof(argv[2]);
    Cgl<10> c(maxiter, density);
    c.prepareGrid();
    c.printGrid();
    c.startCgl();
}
