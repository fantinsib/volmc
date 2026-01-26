/*
 _            _         _____      _           
| |_ ___  ___| |_ ___  | ____|   _| | ___ _ __ 
| __/ _ \/ __| __/ __| |  _|| | | | |/ _ \ '__|
| ||  __/\__ \ |_\__ \ | |__| |_| | |  __/ |   
 \__\___||___/\__|___/ |_____\__,_|_|\___|_|   
                                              
*/



#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>  
#include <stdexcept>
#include "models/black_scholes/black_scholes.hpp"
#include "models/heston/heston.hpp"
#include "schemes/eulerblackscholes.hpp"
#include "schemes/eulerheston.hpp"


TEST_CASE("Scheme - EulerBlackScholes") {

SECTION("Constructor") {

    BlackScholes bs{0.02, 0.2};
    EulerBlackScholes euler_bs{bs};

    std::mt19937 rng;
    float dt = 0.1;
    State init{100};

    State S{euler_bs.step(init, dt, rng)};
    REQUIRE(S.spot());
}
}

TEST_CASE("Scheme - EulerHeston") {

    SECTION("Constructor") {

        Heston heston{0.02, 2, 0.05, 0.4, -0.5};
        EulerHeston euler_heston{heston};

        std::mt19937 rng;
        float dt = 0.1;
        State init{100, 0.2};
        
        State S{euler_heston.step(init, dt, rng)};
        REQUIRE(S.spot());
        REQUIRE(S.vol().has_value());

    }

    SECTION("Constructor error") {

        Heston heston{0.02, 2, 0.05, 0.4, -0.5};
        EulerHeston euler_heston{heston};

        std::mt19937 rng;
        float dt = 0.1;
        State init{100};
        
        REQUIRE_THROWS_AS(euler_heston.step(init, dt, rng), std::logic_error);

    }


}


