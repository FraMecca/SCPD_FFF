#ifndef SETTINGS_HEADER
#define SETTINGS_HEADER

/**
 * This settings file can be used to configure the program according to preferences without
 * the need of command-line parsing.
 * Set the chosen values, then go compile.
 */

#define SEQUENTIAL
//#define STENCIL

#define N_PARTITIONS 4

// #define SCATTER // use mpi_scatterv/gatherv instead of send/recv

// --- CGL

// CGL grid dimension (N -> NxN)
#define DIM 32

// CGL grid area dimension (N -> NxN)
#define SIDE 4

// Maximum number of CGL iterations
#define N_ITERATIONS 20

// --- GA PARAMETERS

// Population size (num. individuals)
#define POPSIZE 500

// Number of generations
#define N_GENERATIONS 10

//Number of grids which the fitness is computed on after N_ITERATIONS iterations (N_FITGRIDS <= N_ITERATIONS)
#define N_FITGRIDS 10

// Survival probability of an individual
const double _survive = 0.05;

// Mutation probability of an individual
const double _mutation = 0.08;

// Percentage of (BEST) individuals which are used for the crossover
const double _crossover = 0.25;

#endif
