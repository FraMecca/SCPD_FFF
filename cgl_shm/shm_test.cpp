#define CATCH_CONFIG_MAIN
#include "../include/catch.hpp"
#include "../libcgl/libcgl.hpp"
#include "partition.hpp"
#include "settings.h"
#include <assert.h>
#include <bitset>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <omp.h>
#include <vector>

#define TSIZE 16 // small enough but partitionable in 8 sub-matrices

TEST_CASE("Partitioning")
{
    SECTION("Filling: the two grids should be equal. (compared element by element)")
    {
        unsigned int maxiter = 20;
        cout << "Testing partition filling on size " << TSIZE << " and " << maxiter << " iterations." << endl;
        Cgl<TSIZE> c = Cgl<TSIZE>(2, maxiter); // side == 2
        c.prepareGrid();
        c.printGrid();
        const auto cgrid = c.getGrid();

        Partition<TSIZE> partitions[N_PARTITIONS];
        fill_partitions(partitions, cgrid);
        cout << endl;
        print_partitions(partitions, false);
        cout << endl;

        int j;
        for (int t = 0; t < N_PARTITIONS; t++) {
            //cout << "--- " << t << endl;
            j = 0;
            //cout << "psize " << partitions[t].psize << ", thread " << t << endl;
            for (int i = partitions[t].start; i < partitions[t].end; i++) {
                //cout << i << " - " << partitions[t].grid->test(j) << ":" << c.grid->test(i) << endl;
                REQUIRE(partitions[t].grid->test(j++) == c.getGrid()->test(i));
            }
        }
    }

    SECTION("Dumping: the two grids should be equal. (compared element by element)")
    {
        unsigned int maxiter = 20;

        cout << "Testing partition dumping on size " << TSIZE << " and " << maxiter << " iterations." << endl;
        Cgl<TSIZE> c = Cgl<TSIZE>(2, maxiter); // side == 2
        c.prepareGrid();
        const auto cgrid = c.getGrid();

        Partition<TSIZE> partitions[N_PARTITIONS];
        fill_partitions(partitions, cgrid);
        cout << endl;

        // dumping
        bitset<TSIZE* TSIZE>* stepGrid = new bitset<TSIZE * TSIZE>();
        dump_partitions(partitions, stepGrid);

        int j;
        for (int t = 0; t < N_PARTITIONS; t++) {
            j = 0;
            for (int i = partitions[t].start; i < partitions[t].end; i++) {
                REQUIRE(partitions[t].grid->test(j++) == stepGrid->test(i));
            }
        }
    }
}
