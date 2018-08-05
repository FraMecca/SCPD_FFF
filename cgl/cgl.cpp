#include <iostream>
#include <assert.h>
#include <bitset>
#include <cstdlib>
#include <random>
#include <vector>
#include <functional>
#include "cgl.hpp"

using namespace std;

template <size_t T>
Cgl<T>::Cgl(unsigned int max_iter) {
    max_iteration = max_iter;
    dim = T;
    density = std::vector<double>();
}

template <size_t T>
Cgl<T>::Cgl(const std::bitset<T*T> init, unsigned int max_iter) {
    max_iteration = max_iter;
    dim = T;
    grid = init;
    density = std::vector<double>();
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
    int neigh[MAX_NEIGH] = {0};
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


//#define getPos(x,y,z) x < 0 || y < 0 || x >= dim || y >= dim ? -1 : (y + x*dim);
inline int getPos(int x, int y, int dim){
  if(x < 0 || y < 0 || x >= dim || y >= dim) return -1 ;
    else return y + x * dim;
}

template <size_t T>
int* Cgl<T>::getNeighbourhood(int x, int y, int* neigh) {
  neigh[0] = getPos(x-1,y-1, dim);
  neigh[1] = getPos(x-1,y, dim);
  neigh[2] = getPos(x-1,y+1, dim);
  neigh[3] = getPos(x,y+1, dim);
  neigh[4] = getPos(x+1,y+1, dim);
  neigh[5] = getPos(x+1,y, dim);
  neigh[6] = getPos(x+1,y-1, dim);
  neigh[7] = getPos(x,y-1, dim);
  for(size_t i = 0; i < MAX_NEIGH; ++i){
    //    std::cout << neigh[i];
    assert(neigh[i]>=-1);
  }
  //cout<< endl;
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
  int pos = getPos(x,y, dim);
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

template <size_t T>
void Cgl<T>::densityScore(int side) {
  assert(side > 0);
  assert(!density.size() != 0);
  density.resize(dim*dim/side);
  /* print array
     for(i: density) cout << i << ' ';
     cout <<endl;
  */
  for(size_t i = 0; i < dim * dim / side; ++i){
      double density_sc = 0.0;
      for(size_t j = 0; j < side; ++j){
        auto pos = i * side + j;
        cout << "i: " << i << " j: " << j <<  " pos: " << pos << " density_sc: " << density_sc << endl; 
        density_sc = grid.test(pos);
      }
      density_sc = density_sc / side;
    density[i] = density_sc;
  }
  /* print array
    for(i: density) cout << i << ' ';
    cout <<endl;
  */
}

double cosine_similarity(std::vector<double> A, std::vector<double>B)
{
  double mul = 0.0;
  double d_a = 0.0;
  double d_b = 0.0 ;

  if (A.size() != B.size())
      throw std::logic_error("Vector A and Vector B are not the same size");

  // Prevent Division by zero
  if (A.size() < 1)
      throw std::logic_error("Vector A and Vector B are empty");

  std::vector<double>::iterator B_iter = B.begin();
  std::vector<double>::iterator A_iter = A.begin();
  for( ; A_iter != A.end(); A_iter++ , B_iter++ ){
      mul += *A_iter * *B_iter;
      d_a += *A_iter * *A_iter;
      d_b += *B_iter * *B_iter;
  }
  if (d_a == 0.0f || d_b == 0.0f)
      throw std::logic_error("cosine similarity is not defined whenever one or both input vectors are zero-vectors.");

  return mul / (sqrt(d_a) * sqrt(d_b));
}
