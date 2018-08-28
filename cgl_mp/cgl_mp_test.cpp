#define CATCH_CONFIG_MAIN
#include <assert.h>
#include <iostream>
#include <bitset>
#include <vector>
#include <cstdlib>
#include <cstdio>
#include <omp.h>
#include "../cgl/cgl.hpp"
#include "partition.hpp"
#include "cgl_mp.hpp"
#include "../include/catch.hpp"

#define TSIZE 16 // small enough but partitionable in 8 sub-matrices

TEST_CASE("Partitioning") {
    SECTION("Filling: the two grids should be equal. (compared element by element)") {
        unsigned int maxiter = 20;
        cout << "Testing partition filling on size " << TSIZE << \
            " and " << maxiter << " iterations." << endl;
        Cgl<TSIZE>* c = new Cgl<TSIZE>(maxiter);
        c->prepareGrid();
        c->printGrid();

        Partition<TSIZE> partitions[N_THREADS];
        fill_partitions(partitions, c->grid);
        cout << endl;
        print_partitions(partitions, false);
        cout << endl;

        int j;
        for(int t=0; t<N_THREADS; t++) {
            //cout << "--- " << t << endl;
            j = 0;
            //cout << "psize " << partitions[t].psize << ", thread " << t << endl;
            for(int i=partitions[t].start; i<partitions[t].end; i++) {
                //cout << i << " - " << partitions[t].grid.test(j) << ":" << c->grid.test(i) << endl;
                REQUIRE(partitions[t].grid.test(j++) == c->grid.test(i));
            }
        }
        delete c;
    }

    SECTION("Dumping: the two grids should be equal. (compared element by element)") {
        unsigned int maxiter = 20;

        cout << "Testing partition dumping on size " << TSIZE << \
            " and " << maxiter << " iterations." << endl;
        Cgl<TSIZE>* c = new Cgl<TSIZE>(maxiter);
        c->prepareGrid();

        Partition<TSIZE> partitions[N_THREADS];
        fill_partitions(partitions, c->grid);
        delete c;
        cout << endl;

        // dumping
        bitset<TSIZE*TSIZE> stepGrid;
        dump_partitions(partitions, stepGrid);

        int j;
        for(int t=0; t<N_THREADS; t++) {
            j = 0;
            for(int i=partitions[t].start; i<partitions[t].end; i++) {
                REQUIRE(partitions[t].grid.test(j++) == stepGrid.test(i));
            }
        }
    }

}
