/*
 _            _         _____      _           
| |_ ___  ___| |_ ___  | ____|   _| | ___ _ __ 
| __/ _ \/ __| __/ __| |  _|| | | | |/ _ \ '__|
| ||  __/\__ \ |_\__ \ | |__| |_| | |  __/ |   
 \__\___||___/\__|___/ |_____\__,_|_|\___|_|   
                                              
*/



#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>  
#include <optional>
#include <stdexcept>
#include "models/black_scholes/black_scholes.hpp"
#include "models/heston/heston.hpp"
#include "schemes/euler.h"
#include "schemes/eulerblackscholes.hpp"
#include "schemes/eulerheston.hpp"


TEST_CASE("Scheme - EulerBlackScholes") {

SECTION("Constructor") {

    BlackScholes bs{0.02, 0.2};
    Euler euler_bs{bs};

    std::mt19937 rng;
    float dt = 0.1;
    State init{100};

    State S{euler_bs.step(init, 0, dt, rng)};
    REQUIRE(S.at(0));
}

SECTION("Init state") {
    BlackScholes bs{0.02, 0.2};
    Euler euler_bs{bs};

    State state = euler_bs.init_state(123.0f, std::nullopt);
    REQUIRE(state.at(0) == Catch::Approx(123.0f));
    //REQUIRE(state.vol().has_value() == false);
}

SECTION("Invalid dt") {
    BlackScholes bs{0.02, 0.2};
    Euler euler_bs{bs};

    std::mt19937 rng;
    State init{100};

    REQUIRE_THROWS_AS(euler_bs.step(init, 0, 0.0f, rng), std::invalid_argument);
    REQUIRE_THROWS_AS(euler_bs.step(init, 0, -0.1f, rng), std::invalid_argument);
}
}

TEST_CASE("Scheme - EulerHeston") {

    SECTION("Constructor") {

        Heston heston{0.02, 2, 0.05, 0.4, -0.5};
        EulerHeston euler_heston{heston};

        std::mt19937 rng;
        float dt = 0.1;
        State init{100, 0.2};
        
        State S = euler_heston.step(init, 0,  dt, rng);
        REQUIRE(S.at(0) >0);
        REQUIRE(S.at(1) >0);
        REQUIRE(S.holds_var());

    }

    SECTION("Constructor error") {

        Heston heston{0.02, 2, 0.05, 0.4, -0.5};
        EulerHeston euler_heston{heston};

        std::mt19937 rng;
        float dt = 0.1;
        State init{100};

        REQUIRE_THROWS_AS(euler_heston.step(init, 0,  dt, rng), std::invalid_argument);

    }

    SECTION("Init state requires v0") {
        Heston heston{0.02, 2, 0.05, 0.4, -0.5};
        EulerHeston euler_heston{heston};
        State init{100};

        REQUIRE_THROWS_AS(euler_heston.init_state(100.0f, std::nullopt), std::invalid_argument);
        State state = euler_heston.init_state(100.0f, 0.2f);
        REQUIRE(state.at(0) == Catch::Approx(100.0f));
        REQUIRE(state.at(1) > 0);
    }

    SECTION("Invalid dt") {
        Heston heston{0.02, 2, 0.05, 0.4, -0.5};
        EulerHeston euler_heston{heston};

        std::mt19937 rng;
        State init{100, 0.2};

        REQUIRE_THROWS_AS(euler_heston.step(init, 0, 0.0f, rng), std::invalid_argument);
        REQUIRE_THROWS_AS(euler_heston.step(init, 0, -0.1f, rng), std::invalid_argument);
    }

}
