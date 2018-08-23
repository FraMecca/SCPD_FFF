#pragma once
#include <bitset>
#include <vector>
using namespace std;

#define N_THREADS 8
/***
 * Two arrays to map the position of the neighbours
 */
const int nX[] = {-1,-1,-1,0,1,1, 1, 0};
const int nY[] = {-1, 0, 1,1,1,0,-1,-1};

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
        bitset<T*(T/N_THREADS+2)> prev;     /** the bitset representing a partition */
        size_t psize;                       /** partition size */
        const size_t size = T*T/N_THREADS;        /** partition size without neighbours */
        bool nIndex[2] = {false, false};           /** indexes of the neighbourhood rows */
        int indexes[2] = {0,0};                /** indexes of the limiting rows */

        /***
         * Compute Cgl iteration for the partition grid
         * Defined in 'cgl_mp.cpp'
         */
        void computeCells(int iter)
        {
            int nrows = psize/T - !nIndex[0] - !nIndex[1];
            bitset<T*(T/N_THREADS + 2)> newGrid;
            for(int x=nIndex[0]; x<nrows-nIndex[1]; x++) {
                for(int y=0; y<T; y++) {
                    updateCell(newGrid,x,y,iter==0);
                }
            }
            prev = grid;
            grid = newGrid;
        }

        /***
         * Compute a single partition by filling the grid in Partition
         * with the corresponding values in Cgl.grid
         */
        void fill(int t, bitset<T*T>& cglGrid)
        {
            int p = 0;
            psize = T*(T/N_THREADS+2);
            thread = t;
            /**
             * considering neighbours (upper / lower limit rows)
             */
            if(thread == 0) {
                start = 0;
                end = (thread+1)*size + T;
                nIndex[1] = true;
                indexes[0] = 0;
                indexes[1] = end/T-1;

            } else if (thread == N_THREADS-1) {
                start = thread*size - T;
                end = (thread+1)*size;
                nIndex[0] = true;
                indexes[0] = start/T+1;
                indexes[1] = end/T;
            } else {
                start = thread*size - T;
                end = (thread+1)*size + T;
                nIndex[0] = nIndex[1] = true;
                indexes[0] = start/T+1;
                indexes[1] = end/T-1;
            }
            assert(start >= 0 && end <= T*T);

            grid.reset();
            for(int i=start; i<end; i++) {
                grid.set(p++, cglGrid.test(i));
            }
        }

        /***
         * Equivalent to the Cgl<T>::printGrid member function
         */
        void printGrid(bool full)
        {
            int nrows = psize/T-!nIndex[0]-!nIndex[1];
            if (full) {
                for (int x=0; x<nrows; x++) {
                    for (int y=0; y<T; y++)
                        cout << grid[y+x*T];
                    cout << endl;
                }
            } else {
                for (int x=nIndex[0]; x<nrows-nIndex[1]; x++) {
                    for (int y=0; y<T; y++)
                        cout << grid[y+x*T];
                    cout << endl;
                }
            }
        }

        /***
         * Dump the grid into a biteset, corresponding rows
         */
        void dumpGrid(bitset<T*T>& dump)
        {
            int nrows = psize/T-!nIndex[0]-!nIndex[1];
            int pos, dumpPos;
            for (int x=nIndex[0]; x<nrows-nIndex[1]; x++) {
                for (int y=0; y<T; y++) {
                    // pos describes the position on the grid
                    pos = getPos(x,y,T);
                    // dumpPos maps the position on the grid to the one of the full grid
                    dumpPos = indexes[0]*T + getPos(x-nIndex[0],y,T);
                    dump.set(dumpPos, grid.test(pos));
                }
            }
        }

    private:

        /***
         * Convert from 2d indexing to 1d indexing
         */
        inline int getPos(int x, int y, int dim)
        {
          if(x < 0 || y < 0 || x >= dim || y >= dim) return -1 ;
            else return y + x * dim;
        }

        /***
         * Check for changes between the previous iteration
         * and the current one
         */
        inline bool isChanged(int i)
        {
            return grid.test(i) != prev.test(i);
        }

        /***
         * Test if a cell has no changes around
         * (if so it can be excluded)
         */
        bool noChanges(int x, int y, int* neighbours)
        {
            if (isChanged(y + x * T))
                return false;
            for (int i=0;i<MAX_NEIGH;++i) {
                if (neighbours[i] != -1 && \
                        isChanged(neighbours[i]))
                    return false;
            }
            return true;
        }

        /***
         * Update a cell computing its neighbours
         * A cell is updated only if changes occourred
         * during the previous iteration
         */
        void updateCell(bitset<T*(T/N_THREADS+2)>& new_grid, int x, int y, bool first)
        {
            // compute neighbours
            // since computation is cheap
            // it is performed every cell considered
            int neighbours[MAX_NEIGH] = {0};
            for(int i=0; i<MAX_NEIGH; i++) {
                neighbours[i] = getPos(x+nX[i], y+nY[i], T);
            }
            for(size_t i = 0; i < MAX_NEIGH; ++i){
                assert(neighbours[i]>=-1);
            }

            //cout << x << " " << y << endl;
            if (!first && noChanges(x,y,neighbours))
                return;

            int alive = 0;
            for (int i=0;i<MAX_NEIGH;++i) {
                if (neighbours[i] != -1 &&
                        grid.test(neighbours[i]))
                    alive++;
            }
            applyRuleOfLife(new_grid,x,y,alive);
        }

        /***
         * Apply rule of life to the given cell (x,y)
         */
        void applyRuleOfLife(bitset<T*(T/N_THREADS+2)>& new_grid, int x, int y, int alive)
        {
            int pos = getPos(x,y,T);
            if (grid.test(pos) && (alive < 2 || alive > 3))
                new_grid.reset(pos);
            else if (grid.test(pos))
                new_grid.set(pos);
            else if (alive == 3)
                new_grid.set(pos);
            else
                new_grid.reset(pos);
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
void print_partitions(Partition<T>* partitions, bool full)
{
    for(int t=0; t<N_THREADS; t++) {
        partitions[t].printGrid(full);
    }
}

template <size_t T>
void dump_partitions(Partition<T>* partitions, bitset<T*T>& stepGrid)
{
    for(int p=0; p<N_THREADS; p++) {
        partitions[p].dumpGrid(stepGrid);
    }
}
