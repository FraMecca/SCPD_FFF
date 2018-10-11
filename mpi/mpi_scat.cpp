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


void routine(mpi::communicator& world)
{
	int nproc = world.size();
	auto rank = world.rank();
	int popPerProc = POPSIZE / (nproc-1);
	int popLeft = POPSIZE % (nproc-1);
	auto sizes = std::vector<int>(world.size());
	auto recv = std::vector<string>(1);

	// Initialize sizes
	sizes[0] = 0;
	for(int i=1;i<sizes.size();++i) {
		sizes[i] = popPerProc;
		if (popLeft > 0) {
			sizes[i]++;
			popLeft--;
		}
	}

	// create target
	vector<double> target = vector<double>(DIM*DIM/(SIDE*SIDE)); //DIM*DIM / side^2;
	for(size_t i = 0; i < target.size()/2; ++i) {
		target[i] = 0.2;
	}
	for(size_t i = target.size()/2; i < target.size(); ++i) {
		target[i] = 0.8;
	}
	
	// master here
	if (rank == 0) {
		//master(world);
		auto people = first_generation();
		auto snd = std::vector<string>();
		auto fitnessSnd = std::vector<double>(1);
		auto fitnessRecv = std::vector<double>(people.size());
		assert(POPSIZE == fitnessRecv.size());

		for (auto el : people)
			snd.push_back(el.getGene()->to_string());
		
		// send to slave
		mpi::scatterv(world, snd, sizes, &recv[0], 0);
		cout << "Master: " << "sent" << endl;
		for (size_t g = 0;g <= N_GENERATIONS;++g) {
			// receive from slaves
			mpi::gatherv(world, fitnessSnd, &fitnessRecv[0], sizes , 0);
			cout << "Master: " << "received" << endl;
			//cout << "fitness: ";
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
			
			// send to slave
			snd.clear();
			recv.clear();
			recv.resize(1);
			for (auto el : people)
				snd.push_back(el.getGene()->to_string());
			assert(snd.size() == POPSIZE);
			mpi::scatterv(world, snd, sizes, &recv[0], 0);
			cout << "Master: " << "sent" << endl;
		}
		cout <<  "Master finito" << endl;
		auto end = std::vector<string>(POPSIZE,"end");
		recv.clear();
		recv.resize(1);
		mpi::scatterv(world, end, sizes, &recv[0], 0);
		return;
	}

	// slave here
	while(true) {
		recv.resize(sizes[rank]);

		// recv from master
		mpi::scatterv(world, &recv[0], sizes[rank], 0);
		cout << rank << ": received" << endl;
		
		if (recv[0] == "end")  {
			cout << rank << " slave finished" << endl;
			return;
		}

		auto fitnessValue = std::vector<double>();
		for (auto el : recv) {
			auto person = Cgl<DIM>(el, SIDE, N_ITERATIONS);
			assert(person.max_iteration > 0);
			person.GameAndFitness(target);
			fitnessValue.push_back(person.fitness);
			person.release();
		}
		assert(fitnessValue.size() == sizes[rank]);
		
		// send to master
		auto fitnessRecv = std::vector<double>(1);
		mpi::gatherv(world, fitnessValue, &fitnessRecv[0], sizes , 0);
		cout << rank << ": sent" << endl;
	}
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
