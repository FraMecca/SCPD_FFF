#include <assert.h>
#include <bitset>
#include <cstdlib>
#include "../cgl/cgl.hpp"
#include "../cgl/cgl.cpp"
#include "partition.hpp"

using namespace std;

int main(int argc, char* argv[])
{
    assert(argc == 2); // max-iter
    unsigned int maxiter = (unsigned int)atoi(argv[1]);

    // init cgl
    Cgl<48>* c = new Cgl<48>(maxiter);
    c->prepareGrid();

    // partition the cgl grid
    Partition<48> partitions[N_THREADS];
    fill_partitions(partitions, c->grid);

    print_partitions(partitions);
    return 0;
}

