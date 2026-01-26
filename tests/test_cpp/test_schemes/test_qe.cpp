/*
 _            _          ___  _____ 
| |_ ___  ___| |_ ___   / _ \| ____|
| __/ _ \/ __| __/ __| | | | |  _|  
| ||  __/\__ \ |_\__ \ | |_| | |___ 
 \__\___||___/\__|___/  \__\_\_____|
*/


#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>  
#include <stdexcept>
#include "models/heston/heston.hpp"
#include "schemes/qe.hpp"


TEST_CASE("Scheme - QE"){

    SECTION("Constructor"){
    Heston heston{0.02, 2, 0.05, 0.4, -0.5};
    QE qe{heston};

    std::mt19937 rng;
    float dt = 0.1;
    State init{100, 0.2};

    State S{qe.step(init, dt, rng)};
    REQUIRE(S.spot());
    }

    SECTION("Constructor Error - Heston"){
{
    Heston heston{0.02, 2, 0.05, 0.4, -0.5};
    QE qe{heston};

    std::mt19937 rng;
    float dt = 0.1;
    State init{100, 0.2};

    State S{qe.step(init, dt, rng)};
    REQUIRE(S.spot());
    }

    }

}