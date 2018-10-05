#include <assert.h>
#include <iostream>
#include <tuple>
#include <mpi.h>
#include "settings.hpp"
#include "libcgl.hpp"
#include "libga.hpp"

using namespace std;

static int world_size = 0;

int next_rank(size_t i)
{
    assert(world_size > 0);
    return i % (world_size - 1) + 1;
}

void send_grid(Cgl<DIM> c, int index)
{
    auto gene = c.getGene()->to_string();
    MPI_Send(gene.c_str(), gene.size(), MPI_CHAR, next_rank(index), 0, MPI_COMM_WORLD);
}

//tuple<std::vector<Cgl<DIM>>, std::vector<MPI_Request*>> first_generation()
std::vector<Cgl<DIM>> first_generation()
{
    auto people = std::vector<Cgl<DIM>>();

    for(size_t i = 0; i < POPSIZE; ++i){
        auto c = Cgl<DIM>(SIDE,N_ITERATIONS);
        c.side = SIDE;
        c.max_iteration = N_ITERATIONS;
        c.prepareGrid();
        people.push_back(c);
        // SEND TO SLAVES here
        send_grid(c, i);
    }
	cout << "master: sent first" << endl;
	MPI_Barrier(MPI_COMM_WORLD);

    assert(people.size() == POPSIZE);
	return people;
}

std::vector<double> recv_fitness()
{
	// len = popsize, init a -1 tutti i campi
    auto fitness = std::vector<double>(POPSIZE, -1);
    //auto requests = std::vector<MPI_Request>(POPSIZE);
	MPI_Request requests[POPSIZE];

    for(int i=0; i<POPSIZE; ++i) {
        //MPI_Irecv(&fitness[i], sizeof(double), MPI_DOUBLE, next_rank(i), 0, MPI_COMM_WORLD, &requests[i]);
        MPI_Recv(&fitness[i], sizeof(double), MPI_DOUBLE, next_rank(i), 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
	MPI_Barrier(MPI_COMM_WORLD);
	cout << "master: received" << endl;

    //MPI_Request* vr = &requests[0]; // from vector to array
    //MPI_Waitall(POPSIZE, requests, MPI_STATUSES_IGNORE);
    return fitness;
}

void master(int rank)
{
    //auto tup = first_generation();
    //auto people = std::get<0>(tup);
    auto people = first_generation();

    for(size_t g = 0; g < N_GENERATIONS; ++g){
        auto fitness = recv_fitness();
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
			//MPI_Request* req = (MPI_Request*)malloc(sizeof(MPI_Request));
			// SEND TO SLAVES here
			send_grid(c, i);
		}
		MPI_Barrier(MPI_COMM_WORLD);
		cout << rank << ": sent" << endl;
    }
    string end = "end";
    MPI_Bcast((void*)end.c_str(), end.size(), MPI_CHAR, 0, MPI_COMM_WORLD);
}


void routine(int rank)
{
    // create target
    vector<double> target = vector<double>(DIM*DIM/(SIDE*SIDE)); //DIM*DIM / side^2;
    for(size_t i = 0; i < target.size()/2; ++i){
        target[i] = 0.2;
    }
    for(size_t i = target.size()/2; i < target.size(); ++i){
        target[i] = 0.8;
    }

    if(rank == 0){
        master(rank);
        return;
    }
    // slave here

    char buf[DIM*DIM];

    while(true) {
        MPI_Recv(buf, DIM*DIM, MPI_CHAR, 0, 0, MPI_COMM_WORLD, NULL);
		MPI_Barrier(MPI_COMM_WORLD);
		cout << rank << ": received" << endl;
        string stbuf = string(buf);
        if (stbuf == "end") {
            return;
        }
        auto person = Cgl<DIM>(stbuf, SIDE, N_ITERATIONS);
        assert(person.max_iteration > 0);
        person.GameAndFitness(target);
        MPI_Send(&person.fitness, sizeof(person.fitness), MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
		MPI_Barrier(MPI_COMM_WORLD);
    }
}

/***
 * Main should not be here, testing purpose
 */
int
main(int argc, char* argv[])
{
    int rank, poolSize;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    routine(rank);

    MPI_Finalize();
    return 0;
}
