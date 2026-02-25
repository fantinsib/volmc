/*
 _            _         _____      _           
| |_ ___  ___| |_ ___  | ____|   _| | ___ _ __ 
| __/ _ \/ __| __/ __| |  _|| | | | |/ _ \ '__|
| ||  __/\__ \ |_\__ \ | |__| |_| | |  __/ |   
 \__\___||___/\__|___/ |_____\__,_|_|\___|_|   
                                              
*/



#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>  
#include <cmath>
#include <memory>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>
#include "engine/montecarlo.hpp"
#include "models/black_scholes/black_scholes.hpp"
#include "models/dupire/dupire.hpp"
#include "models/heston/heston.hpp"
#include "schemes/euler.h"
#include "schemes/eulerheston.hpp"
#include "surface/local_vol.hpp"
#include "types/simulationresult.hpp"


TEST_CASE("Scheme - Euler - BlackScholes") {

SECTION("Constructor") {

    BlackScholes bs{0.02, 0.2};
    Euler euler_bs(std::make_shared<BlackScholes>(bs));

    std::mt19937 rng;
    float dt = 0.1;
    std::pair<double, double> init(100, 0.0) ;

    std::pair<double, double> state = euler_bs.step(init.first, init.second, 0, dt, rng);
    REQUIRE(state.first != 100);
}

SECTION("Init state") {
    BlackScholes bs{0.02, 0.2};
    Euler euler_bs(std::make_shared<BlackScholes>(bs));

    std::pair<double, double> state = euler_bs.init_state(123.0f, std::nullopt);
    REQUIRE(state.first == Catch::Approx(123.0f));
    //REQUIRE(state.vol().has_value() == false);
}

SECTION("Invalid dt") {
    BlackScholes bs{0.02, 0.2};
    Euler euler_bs(std::make_shared<BlackScholes>(bs));

    std::mt19937 rng;
    std::pair<double, double> init{100, 0.0};

    REQUIRE_THROWS_AS(euler_bs.step(init.first, init.second, 0, 0.0f, rng), std::invalid_argument);
    REQUIRE_THROWS_AS(euler_bs.step(init.first, init.second, 0, -0.1f, rng), std::invalid_argument);
}

SECTION("No volatility") {
    BlackScholes bs{0.02, 0};
    Euler euler_bs(std::make_shared<BlackScholes>(bs));

    MonteCarlo engine(euler_bs);
    engine.configure(1, -1);

    size_t n_steps = 5;
    size_t n_paths = 10;

    SimulationResult sim = engine.generate_spot(100,n_steps, 1,n_paths);
    const std::vector<double>& spots = sim.get_paths();
    REQUIRE(spots[0] == 100);
    REQUIRE(spots.size() == (n_steps+1)*n_paths);
    REQUIRE(bs.volatility(10, 100) == 0.0);
    REQUIRE(bs.diffusion(10, 100) == 0.0);
    REQUIRE(sim.avg_terminal_value() ==
        Catch::Approx(100.0 * std::pow(1.0 + 0.02 * (1.0 / (n_steps)), (n_steps))));
}

}

TEST_CASE("Scheme - EulerHeston") {

    SECTION("Constructor") {

        Heston heston{0.02, 2, 0.05, 0.4, -0.5};
        EulerHeston euler_heston{heston};

        std::mt19937 rng;
        float dt = 0.1;
        std::pair<double, double> init{100, 0.2};
        
        std::pair<double, double> state = euler_heston.step(init.first, init.second, 0,  dt, rng);
        REQUIRE(state.first >0);
        REQUIRE(state.first != 100.0);
        REQUIRE(state.second >0);

    }

    SECTION("Constructor error") {

        Heston heston{0.02, 2, 0.05, 0.4, -0.5};
        EulerHeston euler_heston{heston};

        std::mt19937 rng;
        float dt = 0.1;
        std::pair<double, double> init{100, 0.0};

        REQUIRE_THROWS_AS(euler_heston.init_state(init.first, std::nullopt), std::invalid_argument);

    }


    SECTION("Invalid dt") {
        Heston heston{0.02, 2, 0.05, 0.4, -0.5};
        EulerHeston euler_heston{heston};

        std::mt19937 rng;
        std::pair<double, double> init{100, 0.2};

        REQUIRE_THROWS_AS(euler_heston.step(init.first, init.second, 0, 0.0f, rng), std::invalid_argument);
        REQUIRE_THROWS_AS(euler_heston.step(init.first, init.second, 0, -0.1f, rng), std::invalid_argument);
    }

}

std::shared_ptr<LocalVolatilitySurface> make_surface() {
        std::vector<double> t {1,2,3, 4};
        std::vector<double> s {100,200,300};
        std::vector<double> vol{0.20, 0.19, 0.21,   
                                0.22, 0.20, 0.23,   
                                0.25, 0.24, 0.26,
                                0.27, 0.28, 0.29};

        LocalVolatilitySurface surface(t, s, vol);    
        return std::make_shared<LocalVolatilitySurface>(surface);
    }

TEST_CASE("Scheme - Euler - Dupire ") {

SECTION("Constructor") {

    std::shared_ptr<LocalVolatilitySurface> surface = make_surface();
    Dupire dupire(0.03, 0.01, surface);
    Euler euler(std::make_shared<Dupire>(dupire));

    std::mt19937 rng;
    float dt = 0.1;
    std::pair<double, double> init{100, 0.0};

    std::pair<double, double> state = euler.step(init.first, init.second, 0, dt, rng);
    REQUIRE(state.first != 100);
    REQUIRE(state.second == surface->sigma(0, 100));
}

SECTION("Init state") {

    std::shared_ptr<LocalVolatilitySurface> surface = make_surface();
    Dupire dupire(0.03, 0.01, surface);
    Euler euler(std::make_shared<Dupire>(dupire));

    std::pair<double, double> state = euler.init_state(123.0f, 0.0);
    REQUIRE(state.first == Catch::Approx(123.0f));
    //REQUIRE(state.vol().has_value() == false);
}

SECTION("Invalid dt") {
    std::shared_ptr<LocalVolatilitySurface> surface = make_surface();
    Dupire dupire(0.03, 0.01, surface);
    Euler euler(std::make_shared<Dupire>(dupire));

    std::mt19937 rng;
    std::pair<double, double> init{100, 0.2};

    REQUIRE_THROWS_AS(euler.step(init.first,init.second, 0, 0.0f, rng), std::invalid_argument);
    REQUIRE_THROWS_AS(euler.step(init.first, init.second, 0, -0.1f, rng), std::invalid_argument);
}
}
