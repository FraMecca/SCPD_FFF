#ifndef CGL
#define CGL

#include <bitset>

/**
 * This class implements the Conways's Game of Life using a square bitarray as grid.
 *
 */
template<size_t T>
class Cgl {

    private:
        std::bitset<T*T> grid;             /**<The grid*/
        size_t dim;                   /**<Lenght of the grid side*/
        unsigned int max_iteration;   /**<Number of evolution step*/
        float density;                /**<Starging density of the population in the grid*/
        const int MAX_NEIGH = 8;      /**<Max number of neighbour for each cell*/


    public:

        /**
         * Default constructor for the class.
         */
        Cgl(unsigned int max_iter = 10, float dens = 0.5);

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
        size_t getGridSize();

        /**
         * Return the lenght of the side
         */
        size_t getGridSide();

    private:

        /**
         * Update the value in the given position according to its neighbours and the rule of life.
         */
        void updateCell(std::bitset<T*T>& new_grid, int x, int y);

        /**
         * Convert the 2D index on the grid to the 1D bitarray index according to row order transformation.
         */
        int getPos(int x, int y);

        /**
         * Return an array of 1D indexes corresponding to the neighbours of the given cell.
         */
        int* getNeighbourhood(int x, int y);

        /**
         * Apply the rule of life to the given cell.
         */
        void applyRuleOfLife(std::bitset<T*T>& new_grid, int x, int y, int alive);

        /**
         * Copy the values in grid1 to grid2.
         */
        void copyGrid(std::bitset<T*T>& grid1, std::bitset<T*T>& grid2);

};

#endif
