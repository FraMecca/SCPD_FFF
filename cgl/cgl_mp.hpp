/**
 * This module extends the CGL class
 * if the program is compiled with -DPARALLEL
 * The functions added are required to use
 * partitioning instead of sequential CGL computations.
 */
#ifdef PARALLEL

#include <bitset>
#include <vector>
#include "cgl.hpp"

using namespace std;

#define N_THREADS 8
/***
 * Helper function to fill all partitions based on their index
 */
template <size_t T>
void fill_partitions(Cgl<T>* partitions, bitset<T*T>& cglGrid)
{
    for(int t=0; t<N_THREADS; t++) {
       //cout << ">> " << t <<endl;
       partitions[t].fill(t, cglGrid);
       //partitions[t].printGrid(true);
    }
}

/***
 * debugging (print partition grids)
 */
template <size_t T>
void print_partitions(Cgl<T>* partitions, bool full)
{
    for(int t=0; t<N_THREADS; t++) {
        partitions[t].printGrid(full);
    }
}

template <size_t T>
void dump_partitions(Cgl<T>* partitions, bitset<T*T>& stepGrid)
{
    for(int p=0; p<N_THREADS; p++) {
        partitions[p].dumpGrid(stepGrid);
    }
}

#endif
