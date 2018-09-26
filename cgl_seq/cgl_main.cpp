#include <assert.h>
#include "../cgl/cgl.hpp"
#include "../cgl/cgl_eval.hpp"
#include "../include/settings.hpp"

using namespace std;

std::vector<Cgl<DIM>> first_generation()
{
    std::vector<Cgl<DIM>> people = std::vector<Cgl<DIM>>(POPSIZE,Cgl<DIM>(SIDE,N_ITERATIONS));
    for(size_t i = 0; i < people.size(); ++i){
        people[i].side = SIDE;
        people[i].max_iteration = N_ITERATIONS;
        people[i].prepareGrid();
    }
    return people;
}


int main(int argc, char* argv[])
{
    int side = SIDE;

    vector<double> target = vector<double>(DIM*DIM/(side*side)); //DIM*DIM / side^2;
    for(size_t i = 0; i < target.size()/2; ++i){
        target[i] = 0.2;
    }
    for(size_t i = target.size()/2; i < target.size(); ++i){
        target[i] = 0.8;
    }

    vector<Cgl<DIM>> people = first_generation();
    people[0].printGrid();
    for(size_t g = 0; g < N_GENERATIONS; ++g){
        for(size_t i = 0; i < people.size(); ++i){
            people[i].GameAndFitness(target);
        }
        auto grids = Cgl<DIM>::crossover(people, people.size());
        // replace every person with a new person
        for(size_t i = 0; i < people.size(); ++i){
            people[i] = Cgl<DIM>(grids[i],SIDE,N_ITERATIONS);
        }
    }
    people[0].printGrid();


    return 0;
}
