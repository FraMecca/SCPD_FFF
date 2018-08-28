#include <assert.h>
#include <iostream>
#include <bitset>
#include <cstdlib>
#include <cstdio>
#include <omp.h>
#include "../cgl/cgl.hpp"
#include "partition.hpp"

using namespace std;
/***
 * Run using openmp
 * the number of threads is defined as N_THREADS
 */
template <size_t T>
void run_parallel(Partition<T>* parts, unsigned int maxiter)
{
    bitset<T*T> stepGrid;
    dump_partitions(parts, stepGrid);
#ifdef DEBUG
    print_partitions(partitions, false);
    cout << endl;
#endif

    for(int i=0; i<maxiter; i++) {
        // spawn a thread pool
        #pragma omp parallel
        {
            // set thread number dynamically
            omp_set_num_threads(N_THREADS);
            // distribute partitions between threads
            #pragma omp for
            for (int p=0; p<N_THREADS; p++) {
                parts[p].computeCells(i);
            }
        }
        dump_partitions(parts, stepGrid);
        fill_partitions(parts, stepGrid);
#ifdef DEBUG
        print_partitions(parts, false);
        cout << endl;
#endif
        // end debugging
    }
}