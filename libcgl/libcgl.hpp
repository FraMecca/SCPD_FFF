#pragma once
#include <algorithm>
#include <assert.h>
#include <bitset>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <functional>
#include <iostream>
#include <random>
#include <vector>

#include <memory>

using namespace std;

#if defined(STENCIL) && defined(SEQUENTIAL)
#error "Choose a parallelization method"
#endif

#define GRID std::unique_ptr<std::bitset<T * T>>
#define newGRID GRID(new std::bitset<T * T>())
#define strGRID(st) GRID(new std::bitset<T * T>(st))

#define MAX_NEIGH 8 /** Max number of neighbour for each cell*/

#ifdef STENCIL
#include <omp.h>
#endif

#if (DIM % SIDE != 0)
#error "Grid dimension must be compatible with area of submatrix"
#endif

// TIMING FUNCTIONS
#include <chrono>
#include <utility>

class Timer {
private:
    std::chrono::high_resolution_clock::time_point start;
    std::string funName = "";
	std::string line = "";
    std::string logFile = "time.prof";
    bool released = false;

public:
    Timer(std::string f, int l)
    {
        start = std::chrono::high_resolution_clock::now();
        funName = f;
		line = std::to_string(l);
    }

    Timer(std::string f, int l, int r)
    {
        start = std::chrono::high_resolution_clock::now();
        funName = f;
		line = std::to_string(l);
        logFile = "time." + std::to_string(r) + ".prof";
    }

    void endTimer()
    {
        released = true;
        auto end = std::chrono::high_resolution_clock::now();
        assert(end > start);
        auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start)
                           .count();
        //write to log
        std::ofstream out;
        out.open(logFile, std::ios_base::app);
        out << "[" << funName << "]: " << elapsed << std::endl;
    }

    ~Timer()
    {
		if(!released)
			std::cout << "endTimer was not called" + line + funName << std::endl;
        assert(released);
    }
};

#define TIMER auto __timer = Timer(__FUNCTION__, __LINE__)
#define END_TIMER __timer.endTimer()
#define MPI_TIMER auto __timer = Timer(__FUNCTION__, __LINE__, world.rank())

/**
 * Convert from 2d coordinates to 1d
 */
inline int getPos(int x, int y, int dim)
{
    if (x < 0 || y < 0 || x >= dim || y >= dim)
        return -1;
    else
        return y + x * dim;
}

/**
 * This class implements the Conways's Game of Life using a square bitarray as grid.
 *
 */
template <size_t T>
class Cgl {
private:
    size_t dim; /** Lenght of the grid side*/
    GRID gene; /** The initial configuration of the grid */
    GRID grid; /** The grid*/
    short fitnessIterations = N_FITGRIDS; /** Number of iterations in which fitness is computed */
    bool fitnessDone = false; /** flags if fitness has been computed */

public:

    std::vector<double> density; /** A vector of fitness scores for each area **/
    double fitness; /** Fitness related to target density **/
    unsigned int max_iteration = 0; /** Number of evolution step*/
    size_t side = SIDE;

    /**
         * Default constructor for the class.
         */
    Cgl(size_t _side = SIDE, unsigned int max_iter = N_ITERATIONS)
    {
        grid = newGRID;
        gene = newGRID;
        max_iteration = max_iter;
        dim = T;
        density = std::vector<double>();
        fitness = 0.0;
        side = _side;
        prepareGrid();
    }

    /// Constructor for strings
    Cgl(string bitstr, size_t _side = SIDE, unsigned int max_iter = N_ITERATIONS)
    {
        max_iteration = max_iter;
        dim = T;
        grid = strGRID(bitstr);
        gene = strGRID(bitstr);
        density = std::vector<double>();
        fitness = 0.0;
        side = _side;
    }

    /**
         * Prepares the grid with the given values.
         */
    Cgl(GRID init, size_t _side = SIDE, unsigned int max_iter = N_ITERATIONS)
    {
        max_iteration = max_iter;
        dim = T;
        grid = newGRID;
        gene = newGRID;
        copyGrid(init, grid);
        copyGrid(init, gene);
        density = std::vector<double>();
        fitness = 0.0;
        side = _side;
    }

    /**
         * forward to grid->test
         * TODO are we using this
         */
    bool test(size_t pos)
    {
        return grid->test(pos);
    }

    /**
         * forward to grid->set
         * TODO are we using this
         */
    void set(size_t pos)
    {
        grid->set(pos);
    }

    /**
         * Compares two grids
         */
    bool isEqual(GRID g)
    {
        return *g == *grid;
    }

    /**
         * Compares two grids
         */
    bool isEqualGene(GRID g)
    {
        return *g == *gene;
    }

    /**
        * read only gene getter
        * makes a copy
        */
    GRID getGene()
    {
        assert(gene != nullptr);
        GRID ret = newGRID;
        copyGrid(gene, ret);
        return std::move(ret);
    }

    /**
        * read only gene getter to string
        */
    const std::string getGeneString()
    {
        assert(gene != nullptr);
        return gene->to_string();
    }

    /**
         * Initializes the grid according to the given density using a random number generator.
         */
    void prepareGrid()
    {
        assert(side > 0);
        auto bits = randomGrid(side, dim);
        copyGrid(bits, grid);
        copyGrid(bits, gene);
    }

    static GRID randomGrid(int side, size_t dim)
    {
        GRID bits = newGRID;
        std::random_device rd;
        std::uniform_real_distribution<> uni(0.0, 1.0);
        std::mt19937 gen(rd());
        for (size_t line = 0; line < dim / side; ++line) {
            for (size_t col = 0; col < dim / side; ++col) {
                auto distribution = uni(gen);
                std::bernoulli_distribution bern(distribution);
                for (size_t x = 0; x < static_cast<size_t>(side); ++x) {
                    for (size_t y = 0; y < static_cast<size_t>(side); ++y) {
                        auto pos = x * dim + line * (dim * side) + y + col * side;
                        auto c = bern(gen);
                        if (c == 1)
                            bits->set(pos);
                    }
                }
            }
        }
        return bits;
    }

    static std::vector<double> generate_target()
    {
        auto cgl = Cgl();
        cgl.startCgl();
        cgl.densityScore();
        return cgl.density;
    }

    /**
         * Starts the game applying the Rule of Life at each iteration.
         */
#ifdef SEQUENTIAL
    void startCgl(unsigned int n_iter = N_ITERATIONS)
    {
        //TIMER;
        assert(side > 0);
        assert(max_iteration > 0);

        if (n_iter == 0)
            n_iter = max_iteration;
        auto new_grid = newGRID;

        for (size_t i = 0; i < n_iter; ++i) {
            for (size_t x = 0; x < dim; ++x)
                for (size_t y = 0; y < dim; ++y)
                    updateCell(new_grid, x, y);
            copyGrid(new_grid, grid);
        }
    }
#endif

#ifdef STENCIL

    void startCgl(unsigned int n_iter = N_ITERATIONS)
    {
        auto write = newGRID;
        for (size_t i = 0; i < n_iter; i++) {
			// spawn a thread pool
			// set thread number dynamically
			omp_set_dynamic(0);
			omp_set_num_threads(N_PARTITIONS);
			#pragma omp parallel for collapse(2)
			for (size_t x = 0; x < dim; ++x) {
                for (size_t y = 0; y < dim; ++y)
                    updateCell(write, x, y);
			}
        }
        copyGrid(write, grid);
    }
#endif
    /**
        * Starts the game and apply fitness at the last iterations.
        * sets fitnessDone flag to True, it can't be called twice
        */
    void GameAndFitness(std::vector<double> target)
    {
        TIMER;
        assert(side > 0);
        assert(max_iteration > 0);

        if (fitnessDone == true)
            throw std::logic_error("Game of Life and score has already been computed");
        if (max_iteration < static_cast<size_t>(fitnessIterations))
            throw std::logic_error("Invalid number of iterations");

        startCgl(max_iteration - fitnessIterations);
        for (size_t i = 0; i < static_cast<size_t>(fitnessIterations); ++i) {
            startCgl(1);
            fitness += fitnessScore(target);
        }
        fitness = fitness / fitnessIterations;
        fitnessDone = true;
        assert(fitness <= 1.0);
        assert(fitness >= 0.0);
        END_TIMER;
    }

    /**
         * Prints the array visualisation of the grid, from right to left.
         */
    void printGrid()
    {
        assert(dim > 0);
        for (size_t x = 0; x < dim; ++x) {
            for (size_t y = 0; (long long)y < dim; ++y) {
                assert(y >= 0);
                auto pos = y + x * dim;
                if (grid->test(pos) == 0)
                    cout << "_";
                else
                    cout << "X";
            }
            cout << endl;
        }
        cout << endl;
    }

    /**
         * Returns the number of cellc in the grid
         */
    inline size_t getGridSize()
    {
        return grid->size();
    }

    /**
         * Returns the lenght of the side
         */
    inline size_t getGridSide()
    {
        return dim;
    }

    /**
         * Returns the grid
         */
    inline GRID getGrid()
    {
        return grid;
    }

    /**
         * Copy the values in grid1 to grid2.
         */
    static void copyGrid(const GRID& src, GRID& dst)
    {
        assert(src != nullptr && dst != nullptr);
        for (size_t i = 0; i < src->size(); ++i) {
            dst->set(i, src->test(i));
        }
    }

    /**
        * Compute the density of the grid.
        * This function has SIDEEFFECTS.
        */
    void densityScore();

    /**
        * Compute the fitness of the object.
        * This function has SIDEEFFECTS because it calls densityScore;
        */
    double fitnessScore(std::vector<double> target);

    /**
      * Computes crossover from a vector of parents.
      * vector can be sorted and mutation probability specified;
      * Every child has the same number of max_iter (used in constructor) as parents[0];
      * Reuse specifies the probability of a parent of surviving and being included into the next generation instead of yielding a child.
      * Mutation means that a random gene is generated and parents not used.
      * For drawing uses a weighted interval.
      * For the cross over a 4-point non random crossover is used.
      */
    static std::vector<GRID> crossover(std::vector<Cgl<T>>& parents, size_t sz,
        double mutation = _mutation, double survive = _survive, bool shouldSort = true);

private:
    /**
         * Update the value in the given position according to its neighbours and the rule of life.
         */
    void updateCell(GRID& new_grid, int x, int y)
    {
        int neighbours[MAX_NEIGH] = { 0 };
        getNeighbourhood(x, y, neighbours);
        int alive = 0;
        for (size_t i = 0; i < MAX_NEIGH; ++i) {
            if (neighbours[i] == -1)
                continue;
            if (grid->test(neighbours[i]))
                alive++;
        }

        applyRuleOfLife(new_grid, x, y, alive);
    }

    /**
         * Return an array of 1D indexes corresponding to the neighbours of the given cell.
         */
    void getNeighbourhood(int x, int y, int* neigh)
    {
        neigh[0] = getPos(x - 1, y - 1, dim);
        neigh[1] = getPos(x - 1, y, dim);
        neigh[2] = getPos(x - 1, y + 1, dim);
        neigh[3] = getPos(x, y + 1, dim);
        neigh[4] = getPos(x + 1, y + 1, dim);
        neigh[5] = getPos(x + 1, y, dim);
        neigh[6] = getPos(x + 1, y - 1, dim);
        neigh[7] = getPos(x, y - 1, dim);
        for (size_t i = 0; i < MAX_NEIGH; ++i) {
            assert(neigh[i] >= -1);
        }
    }

    /**
         * Apply the rule of life to the given cell.
         */
    void applyRuleOfLife(GRID& new_grid, int x, int y, int alive)
    {
        int pos = getPos(x, y, dim);
        if (grid->test(pos) && (alive < 2 || alive > 3))
            new_grid->reset(pos);
        else if (grid->test(pos))
            new_grid->set(pos);
        else if (alive == 3)
            new_grid->set(pos);
        else
            new_grid->reset(pos);
    }

    /**
         * print the bitset
         */
    friend std::ostream& operator<<(std::ostream& os, const Cgl& m)
    {
        os << m.grid;
        return os;
    }

    /**
        * overload of the < operator, used for sorting a vector of objs
        */
    friend bool operator<(const Cgl& l, const Cgl& r)
    {
        return l.fitness > r.fitness;
    }
};

template <size_t T>
size_t retrieve_parent(std::vector<Cgl<T>>& parents, double choice, size_t pos);
