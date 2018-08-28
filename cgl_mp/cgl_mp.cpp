#include "../cgl/cgl.hpp"
#include "cgl_mp.hpp"
#include "partition.hpp"

using namespace std;
#define SIZE 16

int main(int argc, char* argv[])
{
    assert(argc == 2); // max-iter, par
    unsigned int maxiter = (unsigned int)atoi(argv[1]);

    // init cgl
    Cgl<SIZE>* c = new Cgl<SIZE>(maxiter);
    c->prepareGrid();
    c->printGrid();
    cout << endl;
    cout << "Grid dimension is " << SIZE << " on " << maxiter << " iterations." << endl;
    cout << "Running on " << N_THREADS << " threads." << endl;

    // partition the cgl grid
    Partition<SIZE> partitions[N_THREADS];
    fill_partitions(partitions, c->grid);
    delete c;
    run_parallel(partitions, maxiter);
    return 0;
}
