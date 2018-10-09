#include <assert.h>
#include <iostream>
#include <tuple>
//#include <mpi.h>
#include "settings.hpp"
#include "libcgl.hpp"
#include "libga.hpp"
#include <boost/mpi/environment.hpp>
#include <boost/mpi/communicator.hpp>
#include <boost/mpi/collectives.hpp>
#include <boost/serialization/string.hpp>

namespace mpi = boost::mpi;


int next_rank(size_t i, mpi::communicator& world)
{
    assert(world.size() > 0);
    return i % (world.size() - 1) + 1;
}

void send_grid(Cgl<DIM> c, int index, mpi::communicator& world)
{
    auto gene = c.getGene()->to_string();
	world.send(next_rank(index, world), 0, gene);
}

std::vector<Cgl<DIM>> first_generation(mpi::communicator& world)
{
    auto people = std::vector<Cgl<DIM>>();
	auto grids = std::vector<std::string>();

    for(size_t i = 0; i < POPSIZE; ++i){
        auto c = Cgl<DIM>(SIDE,N_ITERATIONS);
        c.side = SIDE;
        c.max_iteration = N_ITERATIONS;
        c.prepareGrid();
        people.push_back(c);
		grids.push_back(c.getGene()->to_string());
    }
	auto outg = std::vector<std::string>(people.size(),"");
	cout << "scatter" << endl;
	cout << world.size() << endl;
	auto sizes = std::vector<int>(world.size(), 1);
	cout << sizes[3] << endl;
	//sizes[0] = 0;
	mpi::scatterv(world, &grids, sizes, &outg, 0);
	cout << "scatter_" << endl;
	cout << "master: sent first" << endl;

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
	world.barrier();
	cout << "master: received" << endl;

    return fitness;
}

void master(mpi::communicator& world)
{
    //auto tup = first_generation();
    //auto people = std::get<0>(tup);
    auto people = first_generation(world);
	auto rank = world.rank();

    for(size_t g = 0; g < N_GENERATIONS; ++g){
        auto fitness = recv_fitness(world);
        // update population vector with fitness
        for (int i; i<POPSIZE; i++) {
            people[i].fitness = fitness[i];
        }
        assert(POPSIZE == people.size());
		for (int i; i<POPSIZE; i++) {
			cout << fitness[i] << endl;
			cout << people[i].fitness << endl;
			//people[i].fitness = fitness[i];
		}
        auto grids = Cgl<DIM>::crossover(people, people.size());
        // replace every person with a new person
		for(size_t i = 0; i < POPSIZE; ++i){
			auto c = Cgl<DIM>(grids[i],SIDE,N_ITERATIONS);
			// free and reassign
			people[i].release();
			people[i] = c;
			// SEND TO SLAVES here
			send_grid(c, i, world);
		}
		world.barrier();
		cout << rank << ": sent" << endl;
    }
    string end = "end";
	mpi::broadcast(world, end, rank);
}


void routine(mpi::communicator& world)
{
	auto rank = world.rank();

    // create target
    vector<double> target = vector<double>(DIM*DIM/(SIDE*SIDE)); //DIM*DIM / side^2;
    for(size_t i = 0; i < target.size()/2; ++i){
        target[i] = 0.2;
    }
    for(size_t i = target.size()/2; i < target.size(); ++i){
        target[i] = 0.8;
    }

    if(rank == 0){
        master(world);
        return;
    }
    // slave here

    while(true) {
		//string buf;
		auto buf = std::vector<std::string>();
		// recv from master
		mpi::scatterv(world, &buf, 1, 0);
		cout << buf[rank] << endl;

		cout << rank << ": received" << endl;
        if (buf[rank] == "end") {
            return;
        }
        auto person = Cgl<DIM>(buf[rank], SIDE, N_ITERATIONS);
        assert(person.max_iteration > 0);
        person.GameAndFitness(target);
		// send to master
        world.send(0, 0, person.fitness);
		world.barrier();
    }
}

/***
 * Main should not be here, testing purpose
 */
int
main(int argc, char* argv[])
{
    int rank, poolSize;

	mpi::environment env(argc, argv);
	mpi::communicator world;

    routine(world);

    return 0;
}
