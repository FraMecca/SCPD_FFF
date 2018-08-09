#include <assert.h>
#include <iostream>
#include <bitset>
#include <cstdlib>
#include <cstdio>
#include <omp.h>
#include "../cgl/cgl.hpp"
#include "../cgl/cgl.cpp"
#include "partition.hpp"

using namespace std;
#define SIZE 192

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

template <size_t T>
void Partition<T>::computeCells(int iter)
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
 * Update a cell computing its neighbours
 * A cell is updated only if changes occourred
 * during the previous iteration
 */
template <size_t T>
void Partition<T>::updateCell(bitset<T*(T/N_THREADS+2)>& new_grid, int x, int y, bool first)
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

template <size_t T>
void Partition<T>::applyRuleOfLife(bitset<T*(T/N_THREADS+2)>& new_grid, int x, int y, int alive)
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


int main(int argc, char* argv[])
{
    assert(argc == 2); // max-iter, par
    unsigned int maxiter = (unsigned int)atoi(argv[1]);

    // init cgl
    Cgl<SIZE>* c = new Cgl<SIZE>(maxiter);
    c->prepareGrid();
    cout << "Grid dimension is " << SIZE << " on " << maxiter << " iterations." << endl;
#ifndef PARALLEL
        cout << "Running on a single thread." << endl;
        c->startCgl();
#else
        cout << "Running on " << N_THREADS << " threads." << endl;
        // partition the cgl grid
        Partition<SIZE> partitions[N_THREADS];
        fill_partitions(partitions, c->grid);
        // debugging
        // end debugging
        run_parallel(partitions, maxiter);
#endif

    delete c;
    return 0;
}
