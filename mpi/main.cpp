#include <mpi.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

/***
 * Debug purpose (testing mpi)
 */
void
print_rank(int rank)
{
    printf("%d\n", rank);
}

/***
 * Requires argc, argv and a target function.
 * The target function is called with an integer argument (rank)
 */

void
runMPI(int argc, char* argv[], void (*f)(int))
{
    int rank, poolSize;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    f(rank);
    MPI_Finalize();
}

/***
 * Main should not be here, testing purpose
 */
int
main(int argc, char* argv[])
{
    runMPI(argc,argv,print_rank);
    return 0;
}
