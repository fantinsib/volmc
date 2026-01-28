
/*
 _            _                               _                        _       
| |_ ___  ___| |_ ___   _ __ ___   ___  _ __ | |_ ___    ___ __ _ _ __| | ___  
| __/ _ \/ __| __/ __| | '_ ` _ \ / _ \| '_ \| __/ _ \  / __/ _` | '__| |/ _ \ 
| ||  __/\__ \ |_\__ \ | | | | | | (_) | | | | ||  __/ | (_| (_| | |  | | (_) |
 \__\___||___/\__|___/ |_| |_| |_|\___/|_| |_|\__\___|  \___\__,_|_|  |_|\___/ 
                                                                 
 */

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>  
#include "models/black_scholes/black_scholes.hpp"
#include "models/heston/heston.hpp"
#include "schemes/eulerblackscholes.hpp"
#include "schemes/eulerheston.hpp"
#include "schemes/qe.hpp"
#include "engine/montecarlo.hpp"
#include "types/simulationresult.hpp"



TEST_CASE("Monte Carlo - BlackScholes & Euler - basic usage"){

    BlackScholes bs{0.02, 0.1};
    EulerBlackScholes eu_scheme(bs);
    MonteCarlo mc(eu_scheme);

    mc.configure(1);

    Path simulation = mc.simulate_path(100, 252, 1);
    SimulationResult results = mc.generate(100, 252, 1, 10);
    

    REQUIRE(simulation.size() == 253);
    REQUIRE(simulation.end_state().spot() != 100);
    REQUIRE(results.get_npaths() == 10);
    REQUIRE(results.get_npaths() == results.pathbundle->n_paths);
    REQUIRE(results.n_steps == 252);
    REQUIRE(results.n_steps == results.pathbundle->n_steps);
    REQUIRE(results.get_seed() == 1);

    std::vector<float> spots = results.pathbundle->unravel_spot();
    REQUIRE(spots[0]== 100.0f);
    for (float i : spots){
        REQUIRE(i > 0);
    }
        




}

TEST_CASE("Monte Carlo - BlackScholes & Euler - Randomness"){

    BlackScholes bs{0.02, 0.1};
    EulerBlackScholes eu_scheme(bs);
    MonteCarlo mc1(eu_scheme);
    MonteCarlo mc2(eu_scheme);
    MonteCarlo mc3(eu_scheme);

    mc1.configure(1);
    mc2.configure(1);
    mc3.configure(2);

    Path simulation1= mc1.simulate_path(100, 252, 1);
    Path simulation2= mc2.simulate_path(100, 252, 1);
    Path simulation3= mc3.simulate_path(100, 252, 1);

    REQUIRE(simulation1.size() == 253);
    REQUIRE(simulation1.end_state().spot() != 100);
    REQUIRE(simulation2.size() == 253);
    REQUIRE(simulation2.end_state().spot() != 100);
    REQUIRE(simulation3.size() == 253);
    REQUIRE(simulation3.end_state().spot() != 100);

    REQUIRE(mc1.get_seed() == 1);
    REQUIRE(mc2.get_seed() == 1);
    REQUIRE(mc3.get_seed() == 2);
    
    REQUIRE(simulation1.end_state().spot() == simulation2.end_state().spot());
    REQUIRE(simulation1.end_state().spot() != simulation3.end_state().spot());


}


TEST_CASE("Monte Carlo - Heston & Euler - basic usage"){

    Heston heston{0.02, 2, 0.05, 0.4, -0.5};
    EulerHeston eu_scheme(heston);
    MonteCarlo mc(eu_scheme);

    mc.configure(1);

    Path simulation= mc.simulate_path(100, 252, 1, 0.2);

    REQUIRE(simulation.size() == 253);
    REQUIRE(simulation.end_state().spot() != 100);

}

TEST_CASE("Monte Carlo - Heston & Euler - Randomness"){

    Heston heston{0.02, 2, 0.05, 0.4, -0.5};
    EulerHeston eu_scheme(heston);

    MonteCarlo mc1(eu_scheme);
    MonteCarlo mc2(eu_scheme);
    MonteCarlo mc3(eu_scheme);

    mc1.configure(1);
    mc2.configure(1);
    mc3.configure(2);

    Path simulation1= mc1.simulate_path(100, 252, 1, 0.2);
    Path simulation2= mc2.simulate_path(100, 252, 1, 0.2);
    Path simulation3= mc3.simulate_path(100, 252, 1, 0.2);

    REQUIRE(simulation1.size() == 253);
    REQUIRE(simulation1.end_state().spot() != 100);
    REQUIRE(simulation2.size() == 253);
    REQUIRE(simulation2.end_state().spot() != 100);
    REQUIRE(simulation3.size() == 253);
    REQUIRE(simulation3.end_state().spot() != 100);

    REQUIRE(mc1.get_seed() == 1);
    REQUIRE(mc2.get_seed() == 1);
    REQUIRE(mc3.get_seed() == 2);
    
    REQUIRE(simulation1.end_state().spot() == simulation2.end_state().spot());
    REQUIRE(simulation1.end_state().spot() != simulation3.end_state().spot());


}


TEST_CASE("Monte Carlo - QE - basic usage"){

    Heston heston{0.02, 2, 0.05, 0.4, -0.5};
    QE qe(heston);
    MonteCarlo mc(qe);

    mc.configure(1);

    Path simulation= mc.simulate_path(100, 252, 1, 0.2);

    REQUIRE(simulation.size() == 253);
    REQUIRE(simulation.end_state().spot() != 100);

}

TEST_CASE("Monte Carlo - QE - Randomness"){

    Heston heston{0.02, 2, 0.05, 0.4, -0.5};
    EulerHeston qe(heston);
    
    MonteCarlo mc1(qe);
    MonteCarlo mc2(qe);
    MonteCarlo mc3(qe);

    mc1.configure(1);
    mc2.configure(1);
    mc3.configure(2);

    Path simulation1= mc1.simulate_path(100, 252, 1, 0.2);
    Path simulation2= mc2.simulate_path(100, 252, 1, 0.2);
    Path simulation3= mc3.simulate_path(100, 252, 1, 0.2);

    REQUIRE(simulation1.size() == 253);
    REQUIRE(simulation1.end_state().spot() != 100);
    REQUIRE(simulation2.size() == 253);
    REQUIRE(simulation2.end_state().spot() != 100);
    REQUIRE(simulation3.size() == 253);
    REQUIRE(simulation3.end_state().spot() != 100);

    REQUIRE(mc1.get_seed() == 1);
    REQUIRE(mc2.get_seed() == 1);
    REQUIRE(mc3.get_seed() == 2);
    
    REQUIRE(simulation1.end_state().spot() == simulation2.end_state().spot());
    REQUIRE(simulation1.end_state().spot() != simulation3.end_state().spot());


}

TEST_CASE("Monte Carlo - QE - V > 0"){

    Heston heston{0.02, 2, 0.05, 0.4, -0.5};
    QE qe(heston);
    MonteCarlo mc(qe);

    mc.configure(1);

    Path simulation= mc.simulate_path(100, 1000, 1, 0.2);

    REQUIRE(simulation.size() == 1001);
    
    for (State s : simulation){
        REQUIRE(s.vol().has_value());
        REQUIRE(s.vol().value() > 0);
    }

}
