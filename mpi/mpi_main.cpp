#include <assert.h>
#include <iostream>
#include <fstream>
#include <tuple>
//#include <mpi.h>
#include "settings.hpp"
#include "libcgl.hpp"
#include "libga.hpp"
#include <boost/mpi/environment.hpp>
#include <boost/mpi/communicator.hpp>
#include <boost/mpi/collectives.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>

namespace mpi = boost::mpi;


int next_rank(size_t i, mpi::communicator& world)
{
    assert(world.size() > 0);
    return i % (world.size() - 1) + 1;
}


std::vector<Cgl<DIM>> first_generation(mpi::communicator& world)
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

std::vector<double> recv_fitness(mpi::communicator& world)
{
    auto fitness = std::vector<double>(POPSIZE, -1);

    for(int i=0; i<POPSIZE; ++i) {
		// TODO gather
		world.recv(next_rank(i,world), 0, fitness[i]);
    }

    return fitness;
}

int get_next_slave(std::vector<bool>& jobs)
{
	for(size_t i=0; i<jobs.size(); ++i) {
		if(jobs[i] == false) {
			jobs[i] = true; // set to busy
			return i+1;
		}
	}
	return -1;
}

void manage_slaves(mpi::communicator& world, std::vector<Cgl<DIM>>& people)
{
	int nrecv = 0;
	int njobs = 0;
	double f = 0.0f;
	auto jobs = std::vector<bool>(world.size()-1, false);

	while(true) {
		int slave;
		if(nrecv == POPSIZE) break;

		while(njobs < POPSIZE && (slave = get_next_slave(jobs)) != -1) {
			world.send(slave, njobs, people[njobs].getGene()->to_string());
			njobs++;
		}
		auto status = world.recv(mpi::any_source, mpi::any_tag, f);
		people[status.tag()].fitness = f;
		jobs[status.source()-1] = false; // keep track of free slaves

		nrecv++;
	}
}

void master(mpi::communicator& world)
{
    //auto tup = first_generation();
    //auto people = std::get<0>(tup);
    auto people = first_generation(world);
	int cnt = 0;

	while(true) {
		manage_slaves(world, people);
		if (cnt == N_GENERATIONS) break;
		else cnt++;

		// crossover sorts in place according to fitness
        auto grids = Cgl<DIM>::crossover(people, people.size());

		// print the best 5 fitnesses
		cout << cnt << ":\t";
		for(size_t i=0; i<5; i++) {
			cout << people[i].fitness << "\t| ";
		}
		cout << people[POPSIZE-1].fitness << endl;
        // replace every person with a new person
		for(size_t i = 0; i < POPSIZE; ++i){
			auto c = Cgl<DIM>(grids[i],SIDE,N_ITERATIONS);
			// free and reassign
			people[i].release();
			people[i] = c;
		}
	}

    string end = "end";
	mpi::broadcast(world, end, 0);
}


void routine(mpi::communicator& world, std::vector<double> target)
{
	auto rank = world.rank();


    if(rank == 0){
        master(world);
        return;
    }
    // slave here

    while(true) {
		std::string buf;
		// recv from master
		auto status = world.recv(0, mpi::any_tag, buf);

        if (buf == "end") {
            return;
        }
        auto person = Cgl<DIM>(buf, SIDE, N_ITERATIONS);
        assert(person.max_iteration > 0);
        person.GameAndFitness(target);
		// send to master
        world.send(0, status.tag(), person.fitness);
    }
}

/***
 * Main should not be here, testing purpose
 */
int
main(int argc, char* argv[])
{
	if(argc > 1 && std::string(argv[1]) == "--target") {
		// dump to file
		auto density = Cgl<DIM>::generate_target();
		std::ofstream outfile("./target.bin");
		boost::archive::text_oarchive oa(outfile);
		oa & density;
		return 0;
	}
	// load from file
	std::vector<double> target;
	try {
		std::ifstream infile("./target.bin");
		boost::archive::text_iarchive ia(infile);
		ia & target;
	} catch (const::exception &e) {
		cout << "Cannot find target file." << endl;
		return 1;
	}

	mpi::environment env(argc, argv);
	mpi::communicator world;

    routine(world, target);

    return 0;
}
