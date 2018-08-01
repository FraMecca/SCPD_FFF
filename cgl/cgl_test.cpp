#define CATCH_CONFIG_MAIN

#include <iostream>
#include <bitset>
#include <cstdlib>
#include "cgl.hpp"
#include "cgl.cpp"
#include "../include/catch.hpp"

/**
 * Test build for public methods of Cgl class
 */
TEST_CASE("The grid is initialized correctly") {
    SECTION("The grid is set correctly with the default constructor") {
        Cgl<5> c(2,0.5);
        REQUIRE(c.getGridSize() == 25);
        REQUIRE(c.getGridSide() == 5);
    }

    SECTION("The grid is set correctly passing a bitset object") {
        bitset<16> init;
        init.set(0);
        init.set(2);
        init.set(3);
        init.set(6);
        init.set(10);
        init.set(12);
        init.set(14);
        init.set(15);
        Cgl<4> c(init,2,0.5);
        REQUIRE(c.grid.test(0) == 1);
        REQUIRE(c.grid.test(1) == 0);
        REQUIRE(c.grid.test(2) == 1);
        REQUIRE(c.grid.test(3) == 1);
        REQUIRE(c.grid.test(4) == 0);
        REQUIRE(c.grid.test(5) == 0);
        REQUIRE(c.grid.test(6) == 1);
        REQUIRE(c.grid.test(7) == 0);
        REQUIRE(c.grid.test(8) == 0);
        REQUIRE(c.grid.test(9) == 0);
        REQUIRE(c.grid.test(10) == 1);
        REQUIRE(c.grid.test(11) == 0);
        REQUIRE(c.grid.test(12) == 1);
        REQUIRE(c.grid.test(13) == 0);
        REQUIRE(c.grid.test(14) == 1);
        REQUIRE(c.grid.test(15) == 1);
    }
}

TEST_CASE("Iterations are computed correctly") {

    SECTION("One step is computed correctly") {
        bitset<16> init;
        init.set(0);
        init.set(2);
        init.set(3);
        init.set(6);
        init.set(10);
        init.set(12);
        init.set(14);
        init.set(15);
        Cgl<4> c(init,1,0.5);
        c.startCgl();
        REQUIRE(c.grid.test(0) == 0);
        REQUIRE(c.grid.test(1) == 1);
        REQUIRE(c.grid.test(2) == 1);
        REQUIRE(c.grid.test(3) == 1);
        REQUIRE(c.grid.test(4) == 0);
        REQUIRE(c.grid.test(5) == 0);
        REQUIRE(c.grid.test(6) == 1);
        REQUIRE(c.grid.test(7) == 0);
        REQUIRE(c.grid.test(8) == 0);
        REQUIRE(c.grid.test(9) == 0);
        REQUIRE(c.grid.test(10) == 1);
        REQUIRE(c.grid.test(11) == 0);
        REQUIRE(c.grid.test(12) == 0);
        REQUIRE(c.grid.test(13) == 1);
        REQUIRE(c.grid.test(14) == 1);
        REQUIRE(c.grid.test(15) == 1);
    }


    SECTION("Two steps are computed correctly") {
        bitset<16> init;
        init.set(0);
        init.set(2);
        init.set(3);
        init.set(6);
        init.set(10);
        init.set(12);
        init.set(14);
        init.set(15);
        Cgl<4> c(init,2,0.5);
        c.startCgl();
        REQUIRE(c.grid.test(0) == 0);
        REQUIRE(c.grid.test(1) == 1);
        REQUIRE(c.grid.test(2) == 1);
        REQUIRE(c.grid.test(3) == 1);
        REQUIRE(c.grid.test(4) == 0);
        REQUIRE(c.grid.test(5) == 0);
        REQUIRE(c.grid.test(6) == 0);
        REQUIRE(c.grid.test(7) == 0);
        REQUIRE(c.grid.test(8) == 0);
        REQUIRE(c.grid.test(9) == 0);
        REQUIRE(c.grid.test(10) == 0);
        REQUIRE(c.grid.test(11) == 0);
        REQUIRE(c.grid.test(12) == 0);
        REQUIRE(c.grid.test(13) == 1);
        REQUIRE(c.grid.test(14) == 1);
        REQUIRE(c.grid.test(15) == 1);
    }
}

TEST_CASE("copy_grid function") {
    bitset<16> init;
    bitset<16> init2;
    init.set(1);
    init.set(2);
    init.set(3);
    init.set(4);
    init.set(12);
    init.set(15);
    Cgl<4>::copyGrid(init,init2);
    REQUIRE(init2.test(0) == 0);
    REQUIRE(init2.test(1) == 1);
    REQUIRE(init2.test(2) == 1);
    REQUIRE(init2.test(3) == 1);
    REQUIRE(init2.test(4) == 1);
    REQUIRE(init2.test(5) == 0);
    REQUIRE(init2.test(6) == 0);
    REQUIRE(init2.test(7) == 0);
    REQUIRE(init2.test(8) == 0);
    REQUIRE(init2.test(9) == 0);
    REQUIRE(init2.test(10) == 0);
    REQUIRE(init2.test(11) == 0);
    REQUIRE(init2.test(12) == 1);
    REQUIRE(init2.test(13) == 0);
    REQUIRE(init2.test(14) == 0);
    REQUIRE(init2.test(15) == 1);
}
