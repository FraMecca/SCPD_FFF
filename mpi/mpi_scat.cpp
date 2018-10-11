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

std::vector<double> create_target()
{
	auto target = vector<double>(DIM*DIM/(SIDE*SIDE)); //DIM*DIM / side^2;
	for(size_t i = 0; i < target.size()/2; ++i) {
		target[i] = 0.2;
	}
	for(size_t i = target.size()/2; i < target.size(); ++i) {
		target[i] = 0.8;
	}

	return target;
}

std::vector<int> init_sizes(int nproc)
{
	int popPerProc = POPSIZE / (nproc-1); // number of indivuals per slave (lower bound)
	int popLeft = POPSIZE % (nproc-1); // remained individuals to distribute among slaves
	auto sizes = std::vector<int>(nproc);

	sizes[0] = 0;
	for(int i=1;i<sizes.size();++i) {
		sizes[i] = popPerProc;
		if (popLeft > 0) {
			sizes[i]++;
			popLeft--;
		}
	}
	return sizes;
}

std::vector<string> create_snd_grid(std::vector<Cgl<DIM>> people)
{
	auto snd = std::vector<string>();

	for (auto el : people)
		snd.push_back(el.getGene()->to_string());

	return snd;
}

std::vector<double> compute_fitness(std::vector<string> recv, std::vector<double> target)
{
	auto fitnessValue = std::vector<double>();
	for (auto el : recv) {
		auto person = Cgl<DIM>(el, SIDE, N_ITERATIONS);
		assert(person.max_iteration > 0);
		person.GameAndFitness(target);
		fitnessValue.push_back(person.fitness);
		person.release();
	}
	return fitnessValue;
}

void master(mpi::communicator& world, std::vector<int> sizes)
{
	auto people = first_generation();
	auto snd = create_snd_grid(people);
	auto recv = std::vector<string>(1);
	auto fitnessSnd = std::vector<double>(1);
	auto fitnessRecv = std::vector<double>(people.size());
	assert(POPSIZE == fitnessRecv.size());

	// send first gen to slaves
	mpi::scatterv(world, snd, sizes, &recv[0], 0);
	cout << "Master: " << "first gen sent" << endl;

	for (size_t g = 0;g <= N_GENERATIONS;++g) {
		mpi::gatherv(world, fitnessSnd, &fitnessRecv[0], sizes , 0);
		cout << "Master: " << "received fitness" << endl;
		if (g == 0) {
			cout << "FIRST FITNESS: ";
			for (double el : fitnessRecv)
				cout << el << " ";
			cout << endl;
		}

		// update grid with fitness
		for (int i=0;i<people.size();++i)
			people[i].fitness = fitnessRecv[i];

		// last iteration
		if (g == N_GENERATIONS)
			break;

		// crossover
		auto grids = Cgl<DIM>::crossover(people, people.size());
		for(size_t i = 0; i < POPSIZE; ++i) {
			auto c = Cgl<DIM>(grids[i],SIDE,N_ITERATIONS);
			// free and reassign
			people[i].release();
			people[i] = c;
		}

		// send grids to slaves
		snd = create_snd_grid(people);
		assert(snd.size() == POPSIZE);
		mpi::scatterv(world, snd, sizes, &recv[0], 0);
		cout << "Master: " << "sent grids" << endl;
	}

	// iterations finished
	cout <<  "Master finito" << endl;
	auto end = std::vector<string>(POPSIZE,"end");
	mpi::scatterv(world, end, sizes, &recv[0], 0);
	cout << "FITNESS: ";
	for (auto el : people)
		cout << el.fitness << " ";
	cout << endl;
}

void slave(mpi::communicator& world, std::vector<int> sizes, std::vector<double> target)
{
	int rank = world.rank();
	while(true) {
		auto recv = std::vector<string>(sizes[rank]);
		auto fitnessRecv = std::vector<double>(1);

		// recv from master
		mpi::scatterv(world, &recv[0], sizes[rank], 0);
		cout << rank << ": received" << endl;
		// finished iteration
		if (recv[0] == "end")  {
			cout << rank << " slave finished" << endl;
			return;
		}

		auto fitnessValue = compute_fitness(recv,target);
		assert(fitnessValue.size() == sizes[rank]);

		// send fitness to master
		mpi::gatherv(world, fitnessValue, &fitnessRecv[0], sizes , 0);
		cout << rank << ": sent fitness" << endl;
	}
}


void routine(mpi::communicator& world)
{
	auto rank = world.rank();
	// create target
	auto target = create_target(); //DIM*DIM / side^2;
	// init sizes
	auto sizes = init_sizes(world.size());


	if (rank == 0) // master here
		master(world,sizes);
	else // slave here
		slave(world,sizes,target);
}

/***
 * Main should not be here, testing purpose
 */
int
main(int argc, char* argv[])
{

	mpi::environment env(argc, argv);
	mpi::communicator world;

	routine(world);

	return 0;
}
