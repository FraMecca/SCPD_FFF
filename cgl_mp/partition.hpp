#pragma once
#include <bitset>
#include <vector>
using namespace std;

#define N_THREADS 8

/***
 * Partition of the Cgl grid
 * To be used for shared memory parallelization (openmp)
 * The size of the grid includes the rows which are neighbours (+2)
 */
template <size_t T>
struct Partition {
    public:
        int thread;                         /** thread index / partition number */
        int start;                          /** starting index of the partition */
        int end;                            /** ending index of the partition */
        bitset<T*(T/N_THREADS+2)> grid;     /** the bitset representing a partition */
        size_t psize;                       /** partition size */
        size_t size = T*T/N_THREADS;        /** partition size without neighbours */
        int nIndex[2] = {-1, -1};           /** indexes of the neighbourhood rows */

        /***
         * Compute a single partition by filling the grid in Partition
         * with the corresponding values in Cgl.grid
         */
        void fill(int thread, bitset<T*T>& cglGrid)
        {
            grid.reset();
            /**
             * considering neighbours (upper / lower limit rows)
             */
            if(thread == 0) {
                psize = T*(T/N_THREADS+1);
                start = 0;
                end = (thread+1)*size + T;
                nIndex[1] = (thread+1)*(T/N_THREADS);
            } else if (thread == N_THREADS-1) {
                psize = T*(T/N_THREADS+1);
                start = thread*size - T;
                end = (thread+1)*size;
                nIndex[0] = thread*(T/N_THREADS)-1;
            } else {
                psize = T*(T/N_THREADS+2);
                start = thread*size - T;
                end = (thread+1)*size + T;
                nIndex[0] = thread*(T/N_THREADS)-1;
                nIndex[1] = (thread+1)*(T/N_THREADS);
            }

            assert(start >= 0 && end <= T*T);
            int p = 0;
            for(int i=start; i<end; i++) {
                grid.set(p++, cglGrid.test(i));
            }
        }

        /***
         * Equivalent to the Cgl<T>::printGrid member function
         */
        void printGrid() {
            int nrows = psize/T;
            for (int x=0; x<nrows; x++) {
                for (int y=0; y<T; y++)
                    cout << grid[y+x*T];
                cout << endl;
            }
            cout << endl;
        }
};


/***
 * Helper function to fill all partitions based on their index
 */
template <size_t T>
void fill_partitions(Partition<T>* partitions, bitset<T*T>& cglGrid)
{
    for(int t=0; t<N_THREADS; t++) {
       partitions[t].fill(t, cglGrid);
    }
}

/***
 * debugging (print partition grids)
 */
template <size_t T>
void print_partitions(Partition<T>* partitions)
{
    for(int t=0; t<N_THREADS; t++) {
        cout << t << endl;
        partitions[t].printGrid();
        cout << endl;
    }
}
