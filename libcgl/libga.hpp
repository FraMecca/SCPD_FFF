#include <iostream>
#include <assert.h>
#include <bitset>
#include <cstdlib>
#include <random>
#include <vector>
#include <functional>
#include <algorithm>
#include <cstring>
#include <cstdlib>
#include <math.h>

#include "libcgl.hpp"

using namespace std;
template <size_t T>
void Cgl<T>::densityScore() {
    assert(side > 0);
    assert(fitnessDone == false);

    size_t cap = dim*dim/(side*side);
    if(density.capacity() < cap)
        density.resize(cap);

    size_t cnt = 0;

    //To be parallelised?
    for(size_t line = 0; line < dim/side; ++line){
        for(size_t col = 0; col < dim/side; ++col){
            long density_sc = 0;
            for(size_t x = 0; x < side; ++x){
                for(size_t y = 0; y < side ; ++y){
                    auto pos = x*dim + line*(dim*side) + y + col*side;
                    density_sc += grid->test(pos);
                }
            }
            density[cnt] = (double) density_sc / (side*side);
            ++cnt;
        }
    }
    assert(cnt == cap);
}

/**
 * Distanza di Manhattan
 */
double similarity(std::vector<double> A, std::vector<double>B) {
    double res = 0.0;

    if (A.size() != B.size())
        throw std::logic_error("Vector A and Vector B are not the same size: A=" + to_string(A.size()) + " B=" + to_string(B.size()));

    // Prevent Division by zero
    if (A.size() < 1)
        throw std::logic_error("Vector A and Vector B are empty");

    std::vector<double>::iterator B_iter = B.begin();
    std::vector<double>::iterator A_iter = A.begin();
    for( ; A_iter != A.end(); A_iter++ , B_iter++ ){
        auto d = *B_iter - *A_iter;
        res += abs(d);
    }
    auto r = 1.0f - res/A.size();
    return r;
}

template <size_t T>
double Cgl<T>::fitnessScore(std::vector<double> target) {
  densityScore();
  assert(density.size() > 0);
  return similarity(density, target);
}


template <size_t T>
size_t retrieve_parent(std::vector<Cgl<T>>& parents, double choice, size_t pos) {
   assert(parents.size() >0);
    // possible overflow bug in pos == -1
    auto orig_choice = choice;
    for(size_t i = 0; i < parents.size(); ++i){
        if(i == pos) continue;
        assert(parents[i].fitness > 0);
        choice -= parents[i].fitness;
        if(choice <= 0) return i;
    }
    double tot = 0;
    for(size_t i = 0; i < parents.size(); ++i) tot += parents[i].fitness;
    throw std::logic_error("Possible logic error: choice = " + std::to_string(orig_choice) +", pos=" + std::to_string((long long)pos) +",total fitness=" + std::to_string(tot));
}

template <size_t T>
GRID cross_genes(GRID p1, GRID p2) {
    assert(p1->size() == p2->size());
    auto sz = p1->size();
    auto off = sz/4;
    auto res = newGRID;
    for(size_t i=0; i<off; ++i)
        res->set(i,p1->test(i));
    for(size_t i=off; i<off*2; ++i)
        res->set(i,p2->test(i));
    for(size_t i=off*2; i<off*3; ++i)
        res->set(i,p1->test(i));
    for(size_t i=off*3; i<sz; ++i)
        res->set(i,p2->test(i));
    return std::move(res);
}

template <size_t T>
std::vector<GRID> Cgl<T>::crossover(std::vector<Cgl<T>>& parents, size_t sz, double mut, double survive, bool shouldSort) {
    assert(parents.size() > 1);

    std::vector<GRID> results = std::vector<GRID>();
    //A vector of children of the same size as parents, thi is gonna be the new generation
    results.resize(sz);

    //Order pparents, best at the end
    if(shouldSort){
        std::sort(parents.begin(), parents.end());
    }

    //Take the top X parents
    size_t top_n = (size_t)ceil(((double)parents.size())/100*25);
    //when testing on small vectors
    if (top_n < 2)
      top_n = 2;
    std::vector<Cgl<T>> bestParents = std::vector<Cgl<T>>(parents.begin(),parents.begin() + top_n);

    double interval = 0.0;
    for(size_t i=0; i<bestParents.size(); ++i) interval += bestParents[i].fitness;

    // SLOW RETRIEVAL: TODO
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution <> uni_dist(0.0f, 1.0f);
    std::uniform_real_distribution <> dis(0.0f, interval);
    for(size_t i = 0; i < sz; ++i){
        auto shall_mutate = mut > uni_dist(gen);
        if(shall_mutate){
            // random element
            auto c = Cgl<T>::randomGrid(SIDE, bestParents[0].dim);
            results[i] = c;
        } else {
            GRID child_gene;
            auto choice = dis(gen);
            auto p1 = retrieve_parent(bestParents, choice, -1);
            std::uniform_real_distribution <> sdis(0.0f, interval - bestParents[p1].fitness);
            auto d = uni_dist(gen);
            auto shall_reuse = survive > d;
            if(shall_reuse){
                child_gene = bestParents[p1].getGene();
            } else {
                auto choice2 = sdis(gen);
                auto p2 = retrieve_parent(bestParents, choice2, p1);
                child_gene = cross_genes<T>(bestParents[p1].gene, bestParents[p2].gene);
            }
            results[i] = child_gene;
        }
    }
    return results;
}
