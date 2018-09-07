#pragma once
#include <iostream>
#include <assert.h>
#include <cstdlib>
#include <random>
#include <functional>
#include <algorithm>
#include <cstring>
#include <bitset>
#include <vector>
using namespace std;

#define MAX_NEIGH 8      /** Max number of neighbour for each cell*/

/**
 * Convert from 2d coordinates to 1d
 */
inline int getPos(int x, int y, int dim){
  if(x < 0 || y < 0 || x >= dim || y >= dim) return -1 ;
    else return y + x * dim;
}

/**
 * This class implements the Conways's Game of Life using a square bitarray as grid.
 *
 */
template<size_t T>
class Cgl {

    private:
        size_t dim;                   /** Lenght of the grid side*/
        unsigned int max_iteration;   /** Number of evolution step*/
        std::bitset<T*T> gene;             /** The initial configuration of the grid */
        short fitnessIterations = 10;       /** Number of iterations in which fitness is computed */
        bool fitnessDone = false;            /** flags if fitness has been computed */

    public:
        std::bitset<T*T> grid;              /** The grid*/
        //std::bitset<T*T> prev;              /** Grid of the previous iteration*/
        //int neighbours[T*T][MAX_NEIGH];     /** Array of neighbours assigned to each cell*/

        std::vector<double> density;        /** A vector of fitness scores for each area **/
        double fitness;                     /** Fitness related to target density **/

        /**
         * Default constructor for the class.
         */
        Cgl(unsigned int max_iter = 10) {
            max_iteration = max_iter;
            dim = T;
            density = std::vector<double>();
            fitness = 0.0;

            /*for (int x=0;x<dim;++x)
                for (int y=0;y<dim;++y)
                    computeNeighbours(x,y);*/

        }

        /**
         * Prepares the grid with the given values.
         */
        Cgl(const std::bitset<T*T> init, unsigned int max_iter = 10) {
            max_iteration = max_iter;
            dim = T;
            grid = init;
            gene = init;
            density = std::vector<double>();
            fitness = 0.0;

            /*for (int x=0;x<dim;++x)
                for (int y=0;y<dim;++y)
                    computeNeighbours(x,y);*/
        }

        /**
        * read only gene getter
        */
        const bitset<T*T> getGene() {
            return gene;
        }


        /**
        * read only getter for max_iterations.
        */
        const unsigned int getMaxIterations() {
            return max_iteration;
        }

        /**
         * Initializes the grid according to the given density using a random number generator.
         */
        void prepareGrid() {
            std::bitset<T*T> bits;
            std::random_device rd;
            std::mt19937 gen(rd());
            std::bernoulli_distribution d(0.5);
            for(int n = 0; n < bits.size(); ++n){
                bits[n] = d(gen);
            }
            grid = bits;
            gene = bits;

            /*for (int x=0;x<dim;++x)
                for (int y=0;y<dim;++y)
                    prev.reset(y + x * dim);*/
        }


        /**
         * Starts the game applying the Rule of Life at each iteration.
         */
        void startCgl(unsigned int n_iter = 0) {
          if (n_iter == 0) n_iter = max_iteration;
            bitset<T*T> new_grid;

            for (int i=0;i<n_iter;++i) {
                for (int x=0;x<dim;++x)
                    for (int y=0;y<dim;++y)
                        updateCell(new_grid,x,y,i==0);
                //copyGrid(grid,prev);
                copyGrid(new_grid,grid);
                //printGrid();
            }
        }

        /**
        * Starts the game and apply fitness at the last iterations.
        * sets fitnessDone flag to True, it can't be called twice
        */
        void GameAndFitness(int side, std::vector<double> target){
          if (fitnessDone == true)
            throw std::logic_error("Game of Life and score has already been computed");

          startCgl(max_iteration - fitnessIterations);
          for(size_t i = 0; i < fitnessIterations; ++i){
            startCgl(1);
            fitnessScore(side, target);
          }
          fitness = fitness / fitnessIterations;
          fitnessDone = true;
          assert(fitness <= 1.0);
          assert(fitness >= 0.0);
        }


        /**
         * Prints the array visualisation of the grid, from right to left.
         */
        void printGrid() {
            for (int x=0;x<dim;++x) {
                for (int y=0;y<dim;++y)
                    cout << grid[getPos(x,y,dim)];
                cout << endl;
            }
            cout << endl;
        }

        /**
         * Returns the number of cellc in the grid
         */
        inline size_t getGridSize()  {
            return grid.size();
        }

        /**
         * Returns the lenght of the side
         */
        inline size_t getGridSide() {
            return dim;
        }

        /**
         * Returns the grid
         */
        inline bitset<T*T>& getGrid() {
            return grid;
        }


        /**
         * Copy the values in grid1 to grid2.
         */
        static void copyGrid(bitset<T*T>& grid1, bitset<T*T>& grid2) {
            grid2 = grid1;
        }

        /**
        * Compute the density of the grid.
        * This function has SIDEEFFECTS.
        */
        void densityScore(int side);

        /**
        * Compute the fitness of the object.
        * This function has SIDEEFFECTS because it calls densityScore;
        */
        void fitnessScore(int side, std::vector<double> target);

      /**
      * Computes crossover from a vector of parents.
      * vector can be sorted and mutation probability specified;
      * Every child has the same number of max_iter (used in constructor) as parents[0];
      * Reuse specifies the probability of a parent of surviving and being included into the next generation instead of yielding a child.
      * Mutation means that a random gene is generated and parents not used.
      * For drawing uses a weighted interval.
      * For the cross over a 4-point non random crossover is used.
      */
      static std::vector<bitset<T*T>> crossover(std::vector<Cgl<T>> parents, size_t sz,
                                           double mutation = 0.08f, double survive = 0.05f, bool shouldSort = true);


    private:

        /**
         * Update the value in the given position according to its neighbours and the rule of life.
         */
        void updateCell(bitset<T*T>& new_grid, int x, int y, bool first) {
            /*if (!first && noChanges(x,y))
                return;*/
            int neighbours[MAX_NEIGH] = {0};
            getNeighbourhood(x,y,neighbours);
            int alive = 0;
            for (int i=0;i<MAX_NEIGH;++i) {
                if (neighbours[i] == -1)
                    continue;
                if (grid.test(neighbours[i]))
                    alive++;
            }

            applyRuleOfLife(new_grid,x,y,alive);
        }


        /**
         * Convert the 2D index on the grid to the 1D bitarray index according to row order transformation.
         */
        //inline int getPos(int x, int y);

        /**
         * Return an array of 1D indexes corresponding to the neighbours of the given cell.
         */
        int* getNeighbourhood(int x, int y, int* neigh) {
            neigh[0] = getPos(x-1,y-1, dim);
            neigh[1] = getPos(x-1,y, dim);
            neigh[2] = getPos(x-1,y+1, dim);
            neigh[3] = getPos(x,y+1, dim);
            neigh[4] = getPos(x+1,y+1, dim);
            neigh[5] = getPos(x+1,y, dim);
            neigh[6] = getPos(x+1,y-1, dim);
            neigh[7] = getPos(x,y-1, dim);
            for(size_t i = 0; i < MAX_NEIGH; ++i){
                assert(neigh[i]>=-1);
            }
        }


        /**
         * Apply the rule of life to the given cell.
         */
        void applyRuleOfLife(bitset<T*T>& new_grid, int x, int y, int alive) {
            int pos = getPos(x,y,dim);
            if (grid.test(pos) && (alive < 2 || alive > 3))
                new_grid.reset(pos);
            else if (grid.test(pos))
                new_grid.set(pos);
            else if (alive == 3)
                new_grid.set(pos);
            else
                new_grid.reset(pos);
        }

        /**
         * Compute neighbours to a cell
         * by computing their positions on 1 dimension
         * and copying them into the neighbours vector
         */
        /*void computeNeighbours(int x, int y) {
            int neigh[MAX_NEIGH] = {0};
            Cgl<T>::getNeighbourhood(x,y,neigh);
            memcpy(neighbours[y + x * dim],neigh,MAX_NEIGH * sizeof(int));
        }*/

        /**
         * print the bitset
         */
        friend std::ostream& operator<< (std::ostream& os, const Cgl& m){
          os << m.grid;
          return os ;
        }

        /**
        * overload of the < operator, used for sorting a vector of objs
        */
        friend bool operator<(const Cgl& l, const Cgl& r){
          return l.fitness < r.fitness;
        }

        /**
         * Check if cell is changed
         * during the previous iteration
         */
        /*inline bool isChanged(int i) {
            //if (x == -1 && y == -1) return false;
            //cout << i << endl;
            //assert(i<grid.size() && i<prev.size());
            return grid.test(i) != prev.test(i);
        }*/

        /**
         * return true if
         * !isChanged(cell) &&
         * !isChanged(neighbours)
         */
        /*bool noChanges(int x, int y) {
            if (isChanged(y + x * dim))
                return false;

            for (int i=0;i<MAX_NEIGH;++i) {
                if (neighbours[y + x * dim][i] == -1)
                    continue;
                if (isChanged(neighbours[y + x * dim][i]))
                    return false;
            }

            return true;
        }*/
};

template <size_t T>
size_t retrieve_parent(std::vector<Cgl<T>> parents, double choice, size_t pos);
