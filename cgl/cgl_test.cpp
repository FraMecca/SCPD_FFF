#define CATCH_CONFIG_MAIN

#include <iostream>
#include <bitset>
#include <cstdlib>
#include "cgl.hpp"
#include "cgl_eval.hpp"
#include "../include/catch.hpp"

/**
 * Test build for public methods of Cgl class
 */
TEST_CASE("The grid is initialized correctly") {
    SECTION("The grid is set correctly with the default constructor") {
        Cgl<5> c(2);
        REQUIRE(c.getGridSize() == 25);
        REQUIRE(c.getGridSide() == 5);
    }

    SECTION("The grid is set correctly passing a bitset object") {
        bitset<16>* init = new bitset<16>();
        init->set(0);
        init->set(2);
        init->set(3);
        init->set(6);
        init->set(10);
        init->set(12);
        init->set(14);
        init->set(15);
        Cgl<4> c(init,2);
        REQUIRE(c.test(0) == 1);
        REQUIRE(c.test(1) == 0);
        REQUIRE(c.test(2) == 1);
        REQUIRE(c.test(3) == 1);
        REQUIRE(c.test(4) == 0);
        REQUIRE(c.test(5) == 0);
        REQUIRE(c.test(6) == 1);
        REQUIRE(c.test(7) == 0);
        REQUIRE(c.test(8) == 0);
        REQUIRE(c.test(9) == 0);
        REQUIRE(c.test(10) == 1);
        REQUIRE(c.test(11) == 0);
        REQUIRE(c.test(12) == 1);
        REQUIRE(c.test(13) == 0);
        REQUIRE(c.test(14) == 1);
        REQUIRE(c.test(15) == 1);
    }

    SECTION("The grid is set correctly with the random distribution") {
        Cgl<4> c(2);
        c.prepareGrid();
        REQUIRE(c.getGridSize() == 16);
        REQUIRE(c.getGridSide() == 4);
    }
}

TEST_CASE("Iterations are computed correctly") {

    SECTION("One step is computed correctly") {
        bitset<16>* init = new bitset<16>();
        init->set(0);
        init->set(2);
        init->set(3);
        init->set(6);
        init->set(10);
        init->set(12);
        init->set(14);
        init->set(15);
        Cgl<4> c(init,1);
        c.startCgl();
        REQUIRE(c.test(0) == 0);
        REQUIRE(c.test(1) == 1);
        REQUIRE(c.test(2) == 1);
        REQUIRE(c.test(3) == 1);
        REQUIRE(c.test(4) == 0);
        REQUIRE(c.test(5) == 0);
        REQUIRE(c.test(6) == 1);
        REQUIRE(c.test(7) == 0);
        REQUIRE(c.test(8) == 0);
        REQUIRE(c.test(9) == 0);
        REQUIRE(c.test(10) == 1);
        REQUIRE(c.test(11) == 0);
        REQUIRE(c.test(12) == 0);
        REQUIRE(c.test(13) == 1);
        REQUIRE(c.test(14) == 1);
        REQUIRE(c.test(15) == 1);
        REQUIRE(c.isEqualGene(init));
    }


    SECTION("Two steps are computed correctly") {
        bitset<16>* init = new bitset<16>();
        init->set(0);
        init->set(2);
        init->set(3);
        init->set(6);
        init->set(10);
        init->set(12);
        init->set(14);
        init->set(15);
        Cgl<4> c(init,2);
        c.startCgl();
        REQUIRE(c.test(0) == 0);
        REQUIRE(c.test(1) == 1);
        REQUIRE(c.test(2) == 1);
        REQUIRE(c.test(3) == 1);
        REQUIRE(c.test(4) == 0);
        REQUIRE(c.test(5) == 0);
        REQUIRE(c.test(6) == 0);
        REQUIRE(c.test(7) == 0);
        REQUIRE(c.test(8) == 0);
        REQUIRE(c.test(9) == 0);
        REQUIRE(c.test(10) == 0);
        REQUIRE(c.test(11) == 0);
        REQUIRE(c.test(12) == 0);
        REQUIRE(c.test(13) == 1);
        REQUIRE(c.test(14) == 1);
        REQUIRE(c.test(15) == 1);
        REQUIRE(c.isEqualGene(init));
    }
}

//TEST_CASE("copy_grid function") {
    //bitset<16> init;
    //bitset<16> init2;
    //init.set(1);
    //init.set(2);
    //init.set(3);
    //init.set(4);
    //init.set(12);
    //init.set(15);
    //Cgl<4>::copyGrid(init,init2);
    //REQUIRE(init2.test(0) == 0);
    //REQUIRE(init2.test(1) == 1);
    //REQUIRE(init2.test(2) == 1);
    //REQUIRE(init2.test(3) == 1);
    //REQUIRE(init2.test(4) == 1);
    //REQUIRE(init2.test(5) == 0);
    //REQUIRE(init2.test(6) == 0);
    //REQUIRE(init2.test(7) == 0);
    //REQUIRE(init2.test(8) == 0);
    //REQUIRE(init2.test(9) == 0);
    //REQUIRE(init2.test(10) == 0);
    //REQUIRE(init2.test(11) == 0);
    //REQUIRE(init2.test(12) == 1);
    //REQUIRE(init2.test(13) == 0);
    //REQUIRE(init2.test(14) == 0);
    //REQUIRE(init2.test(15) == 1);
//}

TEST_CASE("Density function") {

  SECTION("Compute density for side = 1") {
    bitset<4>* init = new bitset<4>();
    init->set(0);
    init->set(1);
    init->set(2);
    init->set(3);
    Cgl<2> c(init,1);
    REQUIRE(c.density.size() == 0);
    c.densityScore(1);
    REQUIRE(c.density.size() == 4);
    for(auto f: c.density)
      REQUIRE(f == 1.0);
  }
  SECTION("Compute density for side = 2") {
    bitset<4>* init = new bitset<4>();
    init->set(0);
    init->set(1);
    init->set(2);
    init->set(3);
    Cgl<2> c(init,1);
    REQUIRE(c.density.size() == 0);
    c.densityScore(2);
    REQUIRE(c.density.size() == 2);
    for(auto f: c.density)
      REQUIRE(f == 0.5);
  }
}

TEST_CASE("Fitness function") {

  SECTION("Compute fitness for side = 1") {
    bitset<4>* init = new bitset<4>();
    init->set(0);
    init->set(1);
    init->set(2);
    init->set(3);
    Cgl<2> c(init,1);
    std::vector<double> target = std::vector<double>();
    target.push_back(1.0);
    target.push_back(1.0);
    target.push_back(1.0);
    target.push_back(1.0);
    c.fitnessScore(1, target);
    REQUIRE(c.fitness == 1.0);
  }
  SECTION("Compute fitness for side = 2") {
    bitset<4>* init = new bitset<4>();
    init->set(0);
    init->set(1);
    init->set(2);
    init->set(3);
    Cgl<2> c(init,1);
    std::vector<double> target = std::vector<double>();
    target.push_back(1.0);
    target.push_back(1.0);
    c.fitnessScore(2, target);
    REQUIRE(c.fitness == Approx(0.292893));
  }
}

TEST_CASE("Crossover function") {
  SECTION("Retrieve function") {
    bitset<4>* init = new bitset<4>();
    init->set(0);
    init->set(1);
    init->set(2);
    init->set(3);
    //Cgl<2> a(init,1); a.fitness = 0.3;
    //Cgl<2> b(init,1); b.fitness = 0.6;
    std::vector<Cgl<2>> v = std::vector<Cgl<2>>(2);
    v[0].fitness = 0.3;
    v[1].fitness = 0.6;
    //v.push_back(std::move(a));
    //v.push_back(std::move(b));
    //a = nullptr;
    //b = nullptr;
    size_t r = -1;
    r = retrieve_parent(v, 0.5, -1);
    REQUIRE(r == 1);
    r = retrieve_parent(v, 0.0, -1);
    REQUIRE(r == 0);
    REQUIRE_THROWS_AS(retrieve_parent(v, 1.0, -1), std::logic_error);
  }

  SECTION("Cross genes function") {
    auto init = new bitset<4>();
    init->set(0);
    init->set(2);
    auto init2 = new bitset<4>();
    init2->set(1);
    init2->set(3);
    auto child = cross_genes<2>(init, init2); // all ones
    REQUIRE(init->test(0)); REQUIRE(child->test(0)); REQUIRE(!init2->test(0));
    REQUIRE(!init->test(1)); REQUIRE(child->test(1)); REQUIRE(init2->test(1));
    REQUIRE(init->test(2)); REQUIRE(child->test(2)); REQUIRE(!init2->test(2));
    REQUIRE(!init->test(3)); REQUIRE(child->test(3)); REQUIRE(init2->test(3));
    auto init3 = new bitset<4>();
    auto child2 = cross_genes<2>(init, init3); // same ones as init
    REQUIRE(init->test(0)); REQUIRE(child2->test(0)); REQUIRE(!init3->test(0));
    REQUIRE(!init->test(1)); REQUIRE(!child2->test(1)); REQUIRE(!init3->test(1));
    REQUIRE(init->test(2)); REQUIRE(child2->test(2)); REQUIRE(!init3->test(2));
    REQUIRE(!init->test(3)); REQUIRE(!child2->test(3)); REQUIRE(!init3->test(3));
    auto init4 = new bitset<4>();
    auto child3 = cross_genes<2>(init3, init4); // same ones as init
    REQUIRE(!init4->test(0)); REQUIRE(!child3->test(0)); REQUIRE(!init3->test(0));
    REQUIRE(!init4->test(1)); REQUIRE(!child3->test(1)); REQUIRE(!init3->test(1));
    REQUIRE(!init4->test(2)); REQUIRE(!child3->test(2)); REQUIRE(!init3->test(2));
    REQUIRE(!init4->test(3)); REQUIRE(!child3->test(3)); REQUIRE(!init3->test(3));

  }

  SECTION("Compute crossover and reuse") {
    std::vector<Cgl<2>> v = std::vector<Cgl<2>>(2);
    v[0].fitness = 0.3;
    v[1].fitness = 0.7;
    //std::vector<Cgl<2>> v;
    //v.push_back(std::move(a));
    //v.push_back(std::move(b));

    // test that the only parent survived (reuse probability = 1.0)
    auto surv = Cgl<2>::crossover(v, v.size(), 0.0f, 1.0f);
    for(auto i: surv){
      auto testcase = v[0].isEqualGene(i) || v[1].isEqualGene(i);
      REQUIRE(testcase);
        }
    auto res = Cgl<2>::crossover(v, v.size(), 0.0f, 0.0f);
    REQUIRE(res.size() == v.size());
  }
}
