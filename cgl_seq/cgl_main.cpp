#include <assert.h>
#include "../cgl/cgl.hpp"
#include "../cgl/cgl_eval.hpp"
#define DIM 64
#define SIDE 4

using namespace std;

std::vector<Cgl<DIM>> first_generation(unsigned int n_people)
{
    std::vector<Cgl<DIM>> people = std::vector<Cgl<DIM>>(n_people);
    for(size_t i = 0; i < people.size(); ++i){
        people[i].prepareGrid(SIDE);
    }
    return people;
}


int main(int argc, char* argv[])
{
    assert(argc == 4); // max-iter
    unsigned int maxiter = (unsigned int)atoi(argv[1]);
    unsigned int n_people = (unsigned int)atoi(argv[2]);
    unsigned int n_gen = (unsigned int)atoi(argv[3]);
    int side = SIDE;

    vector<double> target = vector<double>(DIM*DIM/(side*side)); //DIM*DIM / side^2;
    for(size_t i = 0; i < target.size()/2; ++i){
        target[i] = 0.0;
    }
    for(size_t i = target.size()/2; i < target.size(); ++i){
        target[i] = 1.0;
    }

    vector<Cgl<DIM>> people = first_generation(n_people);
    people[0].printGrid();
    for(size_t g = 0; g < n_gen; ++g){
        for(size_t i = 0; i < people.size(); ++i){
            people[i].GameAndFitness(side, target);
        }
        auto grids = Cgl<DIM>::crossover(people, people.size());
        // replace every person with a new person
        for(size_t i = 0; i < people.size(); ++i){
            people[i] = Cgl<DIM>(grids[i]);
        }
    }
    people[0].printGrid();


    return 0;
}
