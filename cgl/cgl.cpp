#include <iostream>
#include <assert.h>
#include <bitset>
#include <cstdlib>
#include <random>
#include <vector>
#include <functional>
#include <algorithm>
#include <cstring>
#include "cgl.hpp"

using namespace std;

template <size_t T>
Cgl<T>::Cgl(unsigned int max_iter) {
    max_iteration = max_iter;
    dim = T;
    density = std::vector<double>();
    fitness = 0.0;

    for (int x=0;x<dim;++x)
        for (int y=0;y<dim;++y)
            computeNeighbours(x,y);

}

template <size_t T>
Cgl<T>::Cgl(const std::bitset<T*T> init, unsigned int max_iter) {
    max_iteration = max_iter;
    dim = T;
    grid = init;
    gene = init;
    density = std::vector<double>();
    fitness = 0.0;

    for (int x=0;x<dim;++x)
        for (int y=0;y<dim;++y)
            computeNeighbours(x,y);
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
    gene = bits;

    for (int x=0;x<dim;++x)
        for (int y=0;y<dim;++y)
            prev.reset(y + x * dim);
}

template <size_t T>
void Cgl<T>::startCgl() {
    bitset<T*T> new_grid;

    for (int i=0;i<max_iteration;++i) {
        for (int x=0;x<dim;++x)
            for (int y=0;y<dim;++y)
                updateCell(new_grid,x,y,i==0);
        copyGrid(grid,prev);
        copyGrid(new_grid,grid);
        //printGrid();
    }
}

template <size_t T>
void Cgl<T>::printGrid() {
    for (int x=0;x<dim;++x) {
        for (int y=0;y<dim;++y)
            cout << grid[dim-y + x*dim];
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
void Cgl<T>::updateCell(bitset<T*T>& new_grid, int x, int y, bool first) {
    if (!first && noChanges(x,y))
        return;

    int alive = 0;
    for (int i=0;i<MAX_NEIGH;++i) {
        if (neighbours[y + x * dim][i] == -1)
            continue;
        if (grid.test(neighbours[y + x * dim][i]))
            alive++;
    }

    applyRuleOfLife(new_grid,x,y,alive);
}

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

template <size_t T>
void Cgl<T>::applyRuleOfLife(bitset<T*T>& new_grid, int x, int y, int alive) {
  int pos = getPos(x,y,dim);
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
const bitset<T*T> Cgl<T>::getGene() {
  return gene;
}

template <size_t T>
const unsigned int Cgl<T>::getMaxIterations() {
  return max_iteration;
}

template <size_t T>
inline bitset<T*T>& Cgl<T>::getGrid() {
    return grid;
}

template <size_t T>
inline bool Cgl<T>::isChanged(int i) {
    //if (x == -1 && y == -1) return false;
    //cout << i << endl;
    //assert(i<grid.size() && i<prev.size());
    return grid.test(i) != prev.test(i);
}

template <size_t T>
bool Cgl<T>::noChanges(int x, int y) {
    if (isChanged(y + x * dim))
        return false;

    for (int i=0;i<MAX_NEIGH;++i) {
        if (neighbours[y + x * dim][i] == -1)
            continue;
        if (isChanged(neighbours[y + x * dim][i]))
            return false;
    }

    return true;
}

template <size_t T>
void Cgl<T>::computeNeighbours(int x, int y) {
    int neigh[MAX_NEIGH] = {0};
    Cgl<T>::getNeighbourhood(x,y,neigh);
    memcpy(neighbours[y + x * dim],neigh,MAX_NEIGH * sizeof(int));
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

double similarity(std::vector<double> A, std::vector<double>B) {
  double res = 0.0;

  if (A.size() != B.size())
      throw std::logic_error("Vector A and Vector B are not the same size");

  // Prevent Division by zero
  if (A.size() < 1)
      throw std::logic_error("Vector A and Vector B are empty");

  std::vector<double>::iterator B_iter = B.begin();
  std::vector<double>::iterator A_iter = A.begin();
  for( ; A_iter != A.end(); A_iter++ , B_iter++ ){
    auto d = *B_iter - *A_iter;
    res += d * d;
  }

  return 1.0f - sqrt(res);
}

template <size_t T>
void Cgl<T>::fitnessScore(int side, std::vector<double> target) {
  assert(density.size() == 0);
  assert(fitness == 0);
  densityScore(side);
  /*  print arrayA
     for(i: density) cout << i << ' ';
     cout <<endl;
  */
  assert(density.size() > 0);
  fitness = similarity(density, target);
  cout << "Fitness: "<< fitness << endl;
}


template <size_t T>
size_t retrieve_parent(std::vector<Cgl<T>> parents, double choice, size_t pos) {
  // possible overflow bug in pos == -1
  for(size_t i = 0; i < parents.size(); ++i){
    if(i == pos) continue;
    auto e = parents[i];
    assert(e.fitness > 0);
    choice -= e.fitness;
    if(choice <= 0) return i;
  }
  double tot = 0;
  for(i: parents) tot += i.fitness;
  throw std::logic_error("Possible logic error: choice = " + std::to_string(choice) +", pos=" + std::to_string((long long)pos) +",tot=" + std::to_string(tot));
}

template <size_t T>
std::bitset<T*T> cross_genes(const std::bitset<T*T>& p1, const std::bitset<T*T>& p2) {
  assert(p1.size() == p2.size());
  auto sz = p1.size();
  auto off = sz/4;
  auto res = std::bitset<T*T>();
  for(size_t i=0; i<off; ++i)
    res[i] = p1[i];
  for(size_t i=off; i<off*2; ++i)
    res[i] = p2[i];
  for(size_t i=off*2; i<off*3; ++i)
    res[i] = p1[i];
  for(size_t i=off*3; i<sz; ++i)
    res[i] = p2[i];
  return res;
}

template <size_t T>
std::vector<Cgl<T>> Cgl<T>::crossover(std::vector<Cgl<T>> parents, size_t sz, double mut, double survive, bool shouldSort) {
  assert(parents.size() > 0);
  auto max_iter = parents[0].getMaxIterations();

  std::vector<Cgl<T>> results = std::vector<Cgl<T>>();
  results.resize(sz);

  if(shouldSort)
    std::sort(parents.begin(), parents.end());

  double interval = 0.0;
  for(i: parents) interval += i.fitness;

  // SLOW RETRIEVAL: TODO
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution <> uni_dist(0.0f, 1.0f);
  std::uniform_real_distribution <> dis(0.0f, interval);
  for(size_t i = 0; i < sz; ++i){
    auto shall_mutate = mut > uni_dist(gen);
    if(shall_mutate){
      // random element
      auto c = Cgl(max_iter);
      c.prepareGrid();
      results[i] = c;
    } else {
      std::bitset<T*T> child_gene;
      auto choice = dis(gen);
      cout << "interval: "<< interval << " ";
      cout << "choice: "<< choice << " ";
      auto p1 = retrieve_parent(parents, choice, -1);
      std::uniform_real_distribution <> sdis(0.0f, interval - parents[p1].fitness);
      auto shall_reuse = survive > uni_dist(gen);
      if(shall_reuse){
        child_gene = parents[p1].grid;
      } else {
        auto choice2 = sdis(gen);
        cout << "choice2: "<< choice2 << endl;
        auto p2 = retrieve_parent(parents, choice2, p1);
        child_gene = cross_genes<T>(parents[p1].grid, parents[p2].grid);
      }
      results[i] = Cgl(child_gene, max_iter);
    }
  }
  return results;
}
