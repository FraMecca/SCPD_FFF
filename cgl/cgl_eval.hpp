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
void Cgl<T>::densityScore(int side) {
  assert(side > 0);
  assert(fitnessDone == false);
  if(density.capacity() < dim*dim/side)
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
  assert(fitnessDone == false);
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
  auto orig_choice = choice;
  for(size_t i = 0; i < parents.size(); ++i){
    if(i == pos) continue;
    auto e = parents[i];
    assert(e.fitness > 0);
    choice -= e.fitness;
    if(choice <= 0) return i;
  }
  double tot = 0;
  for(auto i: parents) tot += i.fitness;
  std::cout << choice << std::endl;
  throw std::logic_error("Possible logic error: choice = " + std::to_string(orig_choice) +", pos=" + std::to_string((long long)pos) +",total fitness=" + std::to_string(tot));
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
std::vector<bitset<T*T>> Cgl<T>::crossover(std::vector<Cgl<T>> parents, size_t sz, double mut, double survive, bool shouldSort) {
  assert(parents.size() > 1);
  auto max_iter = parents[0].getMaxIterations();

  std::vector<bitset<T*T>> results = std::vector<bitset<T*T>>();
  results.resize(sz);

  if(shouldSort)
    std::sort(parents.begin(), parents.end());

  double interval = 0.0;
  for(auto i: parents) interval += i.fitness;

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
      results[i] = c.getGene();
    } else {
      std::bitset<T*T> child_gene;
      auto choice = dis(gen);
      cout << "interval: "<< interval << " ";
      cout << "choice: "<< choice << " ";
      auto p1 = retrieve_parent(parents, choice, -1);
      std::uniform_real_distribution <> sdis(0.0f, interval - parents[p1].fitness);
      auto d = uni_dist(gen);
      auto shall_reuse = survive > d;
      if(shall_reuse){
        child_gene = parents[p1].gene;
      } else {
        auto choice2 = sdis(gen);
        cout << " choice2: "<< choice2 << endl;
        auto p2 = retrieve_parent(parents, choice2, p1);
        child_gene = cross_genes<T>(parents[p1].gene, parents[p2].gene);
      }
      results[i] = child_gene;
    }
  }
  return results;
}
