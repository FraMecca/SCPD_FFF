#include <assert.h>
#include <iostream>
#include <tuple>
#include <boost/mpi/environment.hpp>
#include <boost/mpi/communicator.hpp>
#include <boost/mpi/collectives.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
#include "settings.hpp"
#include "libcgl.hpp"
#include "libga.hpp"
#include "../libcgl/util.hpp"

namespace mpi = boost::mpi;

void print_best(std::vector<Cgl<DIM>>& people, int cnt)
{
    cout << cnt << ":\t";
    for(size_t i=0; i<5; i++) {
        cout << people[i].fitness << "\t| ";
    }
    cout << people[POPSIZE-1].fitness << endl;
}


std::vector<int> init_sizes(int nproc)
{
	int popPerProc = POPSIZE / (nproc-1); // number of indivuals per slave (lower bound)
	int popLeft = POPSIZE % (nproc-1); // remained individuals to distribute among slaves
	auto sizes = std::vector<int>(nproc);

	sizes[0] = 0;
	for(size_t i=1;i<sizes.size();++i) {
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

int next_rank(size_t i, mpi::communicator& world)
{
    assert(world.size() > 0);
    return i % (world.size() - 1) + 1;
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

#ifdef SCATTER
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

	for (size_t g = 0;g <= N_GENERATIONS;++g) {
		mpi::gatherv(world, fitnessSnd, &fitnessRecv[0], sizes , 0);
		if (g == 0) {
			cout << "FIRST FITNESS: ";
			for (double el : fitnessRecv)
				cout << el << " ";
			cout << endl;
		}

		// update grid with fitness
		for (size_t i=0;i<people.size();++i)
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
		// finished iteration
		if (recv[0] == "end")  {
			return;
		}

		auto fitnessValue = compute_fitness(recv,target);
		assert(fitnessValue.size() == (size_t)sizes[rank]);

		// send fitness to master
		mpi::gatherv(world, fitnessValue, &fitnessRecv[0], sizes , 0);
	}
}
#else
void master(mpi::communicator& world)
{
    auto people = first_generation();
	int cnt = 0;

	while(true) {
		manage_slaves(world, people);
		if (cnt == N_GENERATIONS) break;
		else cnt++;

		// crossover sorts in place according to fitness
        auto grids = Cgl<DIM>::crossover(people, people.size());

		// print the best 5 fitnesses
        print_best(people, cnt);

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

void slave(mpi::communicator& world, std::vector<double> target)
{
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
#endif

void routine(mpi::communicator& world, std::vector<double> target)
{
	auto rank = world.rank();

    #ifdef SCATTER
	auto sizes = init_sizes(world.size());
    if(rank == 0) master(world, sizes);
    else slave(world, sizes, target);
    #else
    if(rank == 0) master(world);
    else slave(world, target);
    #endif
}

/***
 * Main should not be here, testing purpose
 */
int
main(int argc, char* argv[])
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

	mpi::environment env(argc, argv);
	mpi::communicator world;

    routine(world, target);

    return 0;
}