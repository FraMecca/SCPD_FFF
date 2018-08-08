#pragma once
#include <bitset>
#include <vector>
using namespace std;

#define N_THREADS 8
/***
 * Partition of the Cgl grid
 * To be used for shared memory parallelization (openmp)
 */
template <size_t T>
struct Partition {
    public:
        int thread; /** thread index / partition number */
        int start;  /** starting index of the partition */
        int end;    /** ending index of the partition */
        bitset<T*T/N_THREADS> grid; /** the bitset representing a partition */
        size_t psize = T*T/N_THREADS; /** partition size */

        /***
         * Compute a single partition by filling the grid in Partition
         * with the corresponding values in Cgl.grid
         */
        void fill(int thread, bitset<T*T>& cglGrid) {
            grid.reset();

            start = thread*psize;
            end = (thread+1)*psize;
            assert(start >= 0 && end <= T*T);

            for(int i=0; i<psize; i++) {
                grid.set(i, cglGrid.test(thread*psize + i));
            }
        }

        /***
         * Equivalent to the Cgl<T>::printGrid member function
         */
        void printGrid() {
            for (int x=0;x<T/N_THREADS;x++) {
                for (int y=0;y<T;y++)
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
