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

void send_grid(Cgl<DIM> c, MPI_Request* req, int index)
{
    auto gene = c.getGene()->to_string();
    MPI_Isend(gene.c_str(), gene.size(), MPI_CHAR, next_rank(index), 0, MPI_COMM_WORLD, req);
}

tuple<std::vector<Cgl<DIM>>, std::vector<MPI_Request*>> first_generation()
{
    auto people = std::vector<Cgl<DIM>>();
    auto requests = std::vector<MPI_Request*>();

    for(size_t i = 0; i < POPSIZE; ++i){
        auto c = Cgl<DIM>(SIDE,N_ITERATIONS);
        c.side = SIDE;
        c.max_iteration = N_ITERATIONS;
        c.prepareGrid();
        people.push_back(c);
        // SEND TO SLAVES here
        MPI_Request* req = (MPI_Request*)malloc(sizeof(MPI_Request));
        send_grid(c, req, i);
        requests.push_back(req);
    }
    assert(people.size() == POPSIZE);
    return make_tuple(people, requests);
}

std::vector<double> recv_fitness()
{
    auto fitness = std::vector<double>(-1,POPSIZE);
    auto requests = std::vector<MPI_Request>();

    for(int i=0; i<POPSIZE; ++i) {
        MPI_Irecv(&fitness[i], sizeof(double), MPI_DOUBLE, next_rank(i), 0, MPI_COMM_WORLD, &requests[i]);
    }

    MPI_Request* vr = &requests[0]; // halp
    MPI_Waitall(requests.size(), vr, MPI_STATUSES_IGNORE);
    return fitness;
}

void master()
{
    auto tup = first_generation();
    auto people = std::get<0>(tup);
    auto requests = std::get<1>(tup);

    for(size_t g = 0; g < N_GENERATIONS; ++g){
        auto fitness = recv_fitness();
        // update population vector with fitness
        for (int i; i<POPSIZE; i++) {
            people[i].fitness = fitness[i];
        }
        auto grids = Cgl<DIM>::crossover(people, people.size());
        // replace every person with a new person
        for(size_t i = 0; i < POPSIZE; ++i){
            auto c = Cgl<DIM>(grids[i],SIDE,N_ITERATIONS);
            // free and reassign
            people[i].release();
            people[i] = c;
            MPI_Request* req = (MPI_Request*)malloc(sizeof(MPI_Request));
            // SEND TO SLAVES here
            send_grid(c, req, i);
            // free and reassign
            free(requests[i]);
            requests[i] = req;

        }
        assert(POPSIZE == people.size());
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

    cout << rank << endl;
    if(rank == 0){
        master();
        return;
    }
    // slave here

    char buf[DIM*DIM];

    while(true) {
        MPI_Recv(buf, DIM*DIM, MPI_CHAR, 0, 0, MPI_COMM_WORLD, NULL);
        string stbuf = string(buf);
        if (stbuf == "end") {
            return;
        }
        auto person = Cgl<DIM>(stbuf, SIDE, N_ITERATIONS);
        assert(person.max_iteration > 0);
        person.GameAndFitness(target);
        MPI_Send((void*)&person.fitness, sizeof(person.fitness), MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
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
