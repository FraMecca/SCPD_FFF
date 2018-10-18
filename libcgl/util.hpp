#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
#include <iostream>
#include <fstream>

/** Possible return types for init_target
 * LOADED: Target was loaded from file
 * STORED: Target was generated at runtime, should exit
*/
enum ACTION {LOADED, STORED, ERROR};

/** Main entry point of every main file.
 * Process argv in order to load a target or generate one.
 */
ACTION init_target(int argc, char* argv[], std::vector<double>& target)
{
    if(argc > 1 && std::string(argv[1]) == "--target") {
        // dump to file
        auto density = Cgl<DIM>::generate_target();
        std::ofstream outfile("./target.bin");
        boost::archive::text_oarchive oa(outfile);
        oa & density;
        return STORED;
    }
    // load from file
    std::ifstream infile("./target.bin");
    boost::archive::text_iarchive ia(infile);
    ia & target;
    return LOADED;
}

/** Generate first generation of random individuals
 */
std::vector<Cgl<DIM>> first_generation()
{
    auto people = std::vector<Cgl<DIM>>();

    for(size_t i = 0; i < POPSIZE; ++i){
        auto c = Cgl<DIM>(SIDE,N_ITERATIONS);
        c.side = SIDE;
        c.max_iteration = N_ITERATIONS;
        c.prepareGrid();
        people.push_back(c);
    }
    assert(people.size() == POPSIZE);
	return people;
}
