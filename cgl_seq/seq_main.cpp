#include <assert.h>
#include "settings.hpp"
#include "../libcgl/libcgl.hpp"
#include "../libcgl/libga.hpp"
#include "../libcgl/util.hpp"

int main(int argc, char* argv[])
{
    auto target = std::vector<double>(POPSIZE);
    try{
        auto res = init_target(argc, argv, target);
        if(res == STORED) // target was generated and stored in file
            return 0;
    } catch (std::exception& e) {
        std::cerr << "Error loading target from file" << std::endl;
        return 1;
    }

    vector<Cgl<DIM>> people = first_generation();
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

    return 0;
}
