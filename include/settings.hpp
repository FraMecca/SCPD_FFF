#ifndef SETTINGS_HEADER
#define SETTINGS_HEADER

/**
 * This settings file can be used to configure the program according to preferences without
 * the need of command-line parsing.
 * Set the chosen values, then go compile.
 */

#define SEQUENTIAL
//#define PARTITION

#define N_PARTITIONS 8

// --- CGL

// CGL grid dimension (N -> NxN)
#define DIM 64

// CGL grid area dimension (N -> NxN)
#define SIDE 4

// Maximum number of CGL iterations
#define N_ITERATIONS 100


// --- GA PARAMETERS

// Population size (num. individuals)
#define POPSIZE 200

// Number of generations
#define N_GENERATIONS 100

// Survival probability of an individual
const double _survive = 0.5;

// Mutation probability of an individual
const double _mutation = 0.1;

// Percentage of (BEST) individuals which are used for the crossover
const double _crossover = 0.25;

#endif
