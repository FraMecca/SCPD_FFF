#include <iostream>
#include <bitset>
#include <cstdlib>
#include "cgl.hpp"

using namespace std;

template <size_t T>
Cgl<T>::Cgl(unsigned int max_iter, float dens) {
    max_iteration = max_iter;
    density = dens;
    dim = T;
}

template <size_t T>
void Cgl<T>::prepareGrid() {
    srand(time(NULL));
    for (int i=0;i<grid.size();++i) {
        if (static_cast <float>(rand()) / static_cast <float> (RAND_MAX) <= density)
            grid.set(i);
        else
            grid.reset(i);
    }
}

template <size_t T>
void Cgl<T>::startCgl() {
    bitset<T*T> new_grid;

    for (int i=0;i<max_iteration;++i) {
        for (int k=0;k<new_grid.size();++k)
            new_grid.reset(k);
        for (int x=0;x<dim;++x)
            for (int y=0;y<dim;++y)
                updateCell(new_grid,x,y);

        copyGrid(new_grid,grid);
        printGrid();
    }
}

template <size_t T>
void Cgl<T>::printGrid() {
    cout << grid << endl << endl;
}

template <size_t T>
size_t Cgl<T>::getGridSize()  {
    return grid.size();
}

template <size_t T>
size_t Cgl<T>::getGridSide() {
    return dim;
}

template <size_t T>
void Cgl<T>::updateCell(bitset<T*T>& new_grid, int x, int y) {
    int* neigh = getNeighbourhood(x,y);

    int alive = 0;
    for (int i=0;i<MAX_NEIGH;++i) {
        if (neigh[i] == -1)
            continue;
        if (grid.test(neigh[i]))
            alive++;
    }

    applyRuleOfLife(new_grid,x,y,alive);
    free(neigh);
}

template <size_t T>
int Cgl<T>::getPos(int x, int y) {
    return (x <0 || y < 0 || x >= dim || y >= dim) ? -1 : y + x*dim;
}

template <size_t T>
int* Cgl<T>::getNeighbourhood(int x, int y) {
    int* neigh = (int*) malloc(MAX_NEIGH*sizeof(int));

    neigh[0] = getPos(x-1,y-1);
    neigh[1] = getPos(x-1,y);
    neigh[2] = getPos(x-1,y+1);
    neigh[3] = getPos(x,y+1);
    neigh[4] = getPos(x+1,y+1);
    neigh[5] = getPos(x+1,y);
    neigh[6] = getPos(x+1,y-1);
    neigh[7] = getPos(x,y-1);

    return neigh;
}

        /*int countNeighbour(int x, int y) {
            if ((x == 0 || x = dim-1) && (y == 0 || y == dim-1))
                return THREE_NEIGH;
            if ((x == 0 && y != 0 && y != dim-1) || (y ==0 && x != 0 && x != dim-1))
                return FIVE_NEIGH;
            else
                return EIGHT_NEIGH;
        }*/

template <size_t T>
void Cgl<T>::applyRuleOfLife(bitset<T*T>& new_grid, int x, int y, int alive) {
    int pos = getPos(x,y);
    if (grid.test(pos) && (alive < 2 || alive > 3))
        new_grid.reset(pos);
    else if (grid.test(pos))
        new_grid.set(pos);
    else if (alive == 3)
        new_grid.set(pos);
    else
        new_grid.reset(pos);
}

template <size_t T>
void Cgl<T>::copyGrid(bitset<T*T>& grid1, bitset<T*T>& grid2) {
    for (int i=0;i<grid1.size();++i) {
        if (grid1.test(i))
            grid2.set(i);
        else
            grid2.reset(i);
    }
}

