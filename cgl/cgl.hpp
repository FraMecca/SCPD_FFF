#pragma once
#include <bitset>
#include <vector>

#define MAX_NEIGH 8      /**<Max number of neighbour for each cell*/

/**
 * This class implements the Conways's Game of Life using a square bitarray as grid.
 *
 */
template<size_t T>
class Cgl {

    private:
        size_t dim;                   /**<Lenght of the grid side*/
        unsigned int max_iteration;   /**<Number of evolution step*/

    public:
        std::bitset<T*T> grid;             /**<The grid*/
        std::vector<double> density;        /** A vector of fitness scores for each area **/
        double fitness; /** Fitness related to target density **/

        /**
         * Default constructor for the class.
         */
        Cgl(unsigned int max_iter = 10);

        /**
         * Prepares the grid with the given values.
         */
        Cgl(const std::bitset<T*T> init, unsigned int max_iter = 10);

        /**
         * Initializes the grid according to the given density using a random number generator.
         */
        void prepareGrid();

        /**
         * Starts the game applying the Rule of Life at each iteration.
         */
        void startCgl();

        /**
         * Prints the array visualisation of the grid, from right to left.
         */
        void printGrid();

        /**
         * Returns the number of cellc in the grid
         */
        inline size_t getGridSize();

        /**
         * Returns the lenght of the side
         */
        inline size_t getGridSide();

        /**
         * Returns the grid
         */
        inline std::bitset<T*T>& getGrid();

        /**
         * Copy the values in grid1 to grid2.
         */
        static void copyGrid(std::bitset<T*T>& grid1, std::bitset<T*T>& grid2);

        /**
        * Compute the density of the grid.
        * This function has SIDEEFFECTS.
        * Gives an assert error when called twice.
        */
        void densityScore(int side);

        /**
        * Compute the fitness of the object.
        * This function has SIDEEFFECTS because it calls densityScore;
        * Gives an assert error when called twice.
        */
        void fitnessScore(int side, std::vector<double> target);

  static std::vector<Cgl<T>> crossover(std::vector<Cgl<T>> parents, size_t sz, bool mutation=true);

    private:

        /**
         * Update the value in the given position according to its neighbours and the rule of life.
         */
        void updateCell(std::bitset<T*T>& new_grid, int x, int y);

        /**
         * Convert the 2D index on the grid to the 1D bitarray index according to row order transformation.
         */
        //inline int getPos(int x, int y);

        /**
         * Return an array of 1D indexes corresponding to the neighbours of the given cell.
         */
        int* getNeighbourhood(int x, int y, int* neigh);

        /**
         * Apply the rule of life to the given cell.
         */
        void applyRuleOfLife(std::bitset<T*T>& new_grid, int x, int y, int alive);


        /**
         * print the bitset
         */
        friend std::ostream& operator<< (std::ostream& os, const Cgl& m){
          os << m.grid;
          return os ;
        }
};
