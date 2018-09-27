#include "../cgl/cgl.hpp"
#include "cgl_mp.hpp"
#include "partition.hpp"
#include "../include/settings.hpp"

using namespace std;
#define SIZE 16

int main(int argc, char* argv[])
{

    // init cgl
    //Cgl<SIZE>* c = new Cgl<SIZE>(N_ITERATIONS);
    Cgl<DIM> c;
    c.prepareGrid();
    c.printGrid();
    cout << endl;
    cout << "Grid dimension is " << DIM << " on " << N_ITERATIONS << " iterations." << endl;
    cout << "Running on " << N_PARTITIONS << " threads." << endl;

    // partition the cgl grid
    Partition<DIM> partitions[N_PARTITIONS];
    const auto cgrid = c.getGrid();
    fill_partitions(partitions, cgrid);
    run_parallel(partitions, N_ITERATIONS);
    c.release();
    return 0;
}
