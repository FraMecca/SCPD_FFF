#include <assert.h>
#include "settings.hpp"
#include "../libcgl/libcgl.hpp"
#include "../libcgl/libga.hpp"
#include "../libcgl/util.hpp"

int main(int argc, char* argv[])
{
    auto target = std::vector<double>(DIM*DIM/(SIDE*SIDE));
    try{
        auto res = init_target(argc, argv, target, 0);
        if(res == STORED) // target was generated and stored in file
            return 0;
    } catch (std::exception& e) {
        std::cerr << "Error loading target from file" << std::endl;
        return 1;
    }

    auto people = first_generation();
    for(size_t g = 0; g < N_GENERATIONS; ++g){
        for(size_t i = 0; i < people.size(); ++i){
            people[i].GameAndFitness(target);
        }
        auto grids = Cgl<DIM>::crossover(people, people.size());
		    print_best(people, g);
        // replace every person with a new person
        for(size_t i = 0; i < people.size(); ++i){
            people[i] = Cgl<DIM>(std::move(grids[i]),SIDE,N_ITERATIONS);
        }
    }

    return 0;
}
