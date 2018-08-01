#include <iostream>
#include <assert.h>
#include <bitset>
#include <cstdlib>
#include <random>
#include <vector>
#include "cgl.hpp"

using namespace std;

template <size_t T>
Cgl<T>::Cgl(unsigned int max_iter, float dens) {
    max_iteration = max_iter;
    density = dens;
    dim = T;
}

template <size_t T>
Cgl<T>::Cgl(const std::bitset<T*T> init, unsigned int max_iter, float dens) {
    max_iteration = max_iter;
    density = dens;
    dim = T;
    grid = init;
}

template <size_t T>
void Cgl<T>::prepareGrid() {
    std::bitset<T*T> bits;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::bernoulli_distribution d(0.5);
    for(int n = 0; n < bits.size(); ++n){
      bits[n] = d(gen);
    }
    grid = bits;
}

template <size_t T>
void Cgl<T>::startCgl() {
    bitset<T*T> new_grid;

    for (int i=0;i<max_iteration;++i) {
        for (int x=0;x<dim;++x)
            for (int y=0;y<dim;++y)
                updateCell(new_grid,x,y);

        copyGrid(new_grid,grid);
        printGrid();
    }
}

template <size_t T>
void Cgl<T>::printGrid() {
    for (int j=0;j<dim;++j) {
        for (int i=0;i<dim;++i)
            cout << grid[dim-i + j*dim];
        cout << endl;
    }
    cout << endl;
}

template <size_t T>
inline size_t Cgl<T>::getGridSize()  {
    return grid.size();
}

template <size_t T>
inline size_t Cgl<T>::getGridSide() {
    return dim;
}

template <size_t T>
void Cgl<T>::updateCell(bitset<T*T>& new_grid, int x, int y) {
    int neigh[MAX_NEIGH];
    getNeighbourhood(x,y,(int*)neigh);

    int alive = 0;
    for (int i=0;i<MAX_NEIGH;++i) {
        if (neigh[i] == -1)
            continue;
        if (grid.test(neigh[i]))
            alive++;
    }

    applyRuleOfLife(new_grid,x,y,alive);
}


template <size_t T>
inline int Cgl<T>::getPos(int x, int y) {
    return (x < 0 || y < 0 || x >= dim || y >= dim) ? -1 : y + x*dim;
}

//#define getPos(x,y) (x < 0 || y < 0 || x >= dim || y >= dim) ? -1 : y + x*dim;

template <size_t T>
int* Cgl<T>::getNeighbourhood(int x, int y, int* neigh) {

    neigh[0] = getPos(x-1,y-1);
    neigh[1] = getPos(x-1,y);
    neigh[2] = getPos(x-1,y+1);
    neigh[3] = getPos(x,y+1);
    neigh[4] = getPos(x+1,y+1);
    neigh[5] = getPos(x+1,y);
    neigh[6] = getPos(x+1,y-1);
    neigh[7] = getPos(x,y-1);
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
  grid2 = grid1;
}

template <size_t T>
inline bitset<T*T>& Cgl<T>::getGrid() {
    return grid;
}

/*int main(int argc, char* argv[]) {
    assert(argc == 3); // dim, max-iter, density
    unsigned int maxiter = (unsigned int)atoi(argv[1]);
    float density = (float)atof(argv[2]);
    Cgl<10> c(maxiter, density);
    c.prepareGrid();
    c.printGrid();
    c.startCgl();
}*/

