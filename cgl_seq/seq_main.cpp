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
			      people[i].release();
            people[i] = Cgl<DIM>(grids[i],SIDE,N_ITERATIONS);
        }
        //people[0].printGrid();
        // FREEING MEMORY
        for (std::vector<std::bitset<DIM*DIM>*>::iterator i = grids.begin(); i != grids.end(); ++i)
            delete (*i);
        grids.clear();
        //people[0].printGrid();
    }

	// free memory allocated by grids and genes
	for(size_t i = 0; i < people.size(); ++i){
		people[i].release();
	}

    return 0;
}
