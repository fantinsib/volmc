
/*
 _            _                               _                        _       
| |_ ___  ___| |_ ___   _ __ ___   ___  _ __ | |_ ___    ___ __ _ _ __| | ___  
| __/ _ \/ __| __/ __| | '_ ` _ \ / _ \| '_ \| __/ _ \  / __/ _` | '__| |/ _ \ 
| ||  __/\__ \ |_\__ \ | | | | | | (_) | | | | ||  __/ | (_| (_| | |  | | (_) |
 \__\___||___/\__|___/ |_| |_| |_|\___/|_| |_|\__\___|  \___\__,_|_|  |_|\___/ 
                                                                 
 */

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>  
#include <memory>
#include <random>
#include "models/black_scholes/black_scholes.hpp"
#include "models/heston/heston.hpp"
#include "models/dupire/dupire.hpp"
#include "schemes/eulerblackscholes.hpp"
#include "schemes/eulerheston.hpp"
#include "schemes/eulerdupire.hpp"
#include "schemes/qe.hpp"
#include "engine/montecarlo.hpp"
#include "types/simulationresult.hpp"
#include "surface/local_vol.hpp"



TEST_CASE("Monte Carlo - BlackScholes & Euler - basic usage"){

    BlackScholes bs{0.02, 0.1};
    EulerBlackScholes eu_scheme(bs);
    MonteCarlo mc(eu_scheme);

    mc.configure(1);

    std::mt19937 rng;

    Path simulation = mc.simulate_path(100, 252, 1, rng);
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

TEST_CASE("Monte Carlo - Randomness with no seed") {
    BlackScholes bs{0.02, 0.1};
    EulerBlackScholes eu_scheme(bs);
    MonteCarlo mc(eu_scheme);

    SimulationResult sim1 = mc.generate(100, 252, 1, 1);
    SimulationResult sim2 = mc.generate(100, 252, 1, 1);

    std::vector<float> spots1 = sim1.pathbundle->unravel_spot();
    std::vector<float> spots2 = sim2.pathbundle->unravel_spot();

    REQUIRE(spots1.size() == spots2.size());
    
    for (size_t i = 1; i < spots1.size(); i++){
        REQUIRE(spots1[i] != spots2[i]);
    };
    

}

TEST_CASE("Monte Carlo - BlackScholes & Euler - Randomness"){

    BlackScholes bs{0.02, 0.1};
    EulerBlackScholes eu_scheme(bs);
    MonteCarlo mc1(eu_scheme);
    MonteCarlo mc2(eu_scheme);
    MonteCarlo mc3(eu_scheme);

    std::mt19937 rng;
    mc1.configure(1);
    SimulationResult simulation1= mc1.generate(100, 252, 1, 1);
    mc2.configure(1);
    SimulationResult simulation2= mc2.generate(100, 252, 1, 1);
    mc3.configure(2);
    SimulationResult simulation3= mc3.generate(100, 252, 1, 1);
    SimulationResult simulation4= mc3.generate(100, 252, 1, 1);

    // The 2 separately instanciated MC generators with the same seed should yield the same result, and 
    // a different one from another MC generator with a different seed. 
    // however, the same same generator called twice with a fixed seed should give different results

    REQUIRE(simulation1.get_nsteps() == 252);
    REQUIRE(simulation1.avg_terminal_value() != 100);
    REQUIRE(simulation2.get_nsteps() == 252);
    REQUIRE(simulation2.avg_terminal_value() != 100);
    REQUIRE(simulation3.get_nsteps() == 252);
    REQUIRE(simulation3.avg_terminal_value() != 100);
    REQUIRE(simulation4.get_nsteps() == 252);
    REQUIRE(simulation4.avg_terminal_value() != 100);

    REQUIRE(mc1.get_seed() == 1);
    REQUIRE(mc2.get_seed() == 1);
    REQUIRE(mc3.get_seed() == 2);

    std::vector<float> spots1 = simulation1.pathbundle->unravel_spot();
    std::vector<float> spots2 = simulation2.pathbundle->unravel_spot();
    std::vector<float> spots3 = simulation3.pathbundle->unravel_spot();
    std::vector<float> spots4 = simulation4.pathbundle->unravel_spot();

    // 1 and 2 must have the same path because they are both generated from the
    // same rng and same seed
    // 1 and 3 must differ because they are generated from distinct seed
    // 3 and 4 must differ because they are generated from the same seed 
    // but from a rng in a different state 
    
    bool sim2_3_diff = false;
    bool sim3_4_diff = false;
    for (size_t i=0; i < spots1.size(); i++){
        REQUIRE(spots1[i] == spots2[i]);
        if (spots2[i] != spots3[i]) {sim2_3_diff = true;}
        if (spots3[i] != spots4[i]) {sim3_4_diff = true;}
    }
    REQUIRE(sim2_3_diff);
    REQUIRE(sim3_4_diff);

}

TEST_CASE("Monte Carlo - Reset RNG and seed") {
    BlackScholes bs{0.02, 0.2};
    EulerBlackScholes eu_bs(bs);
    MonteCarlo mc(eu_bs);

    SECTION("Reset RNG") {
        mc.configure(1);
        SimulationResult sim1 = mc.generate(100, 252, 1, 1);
        mc.reset_rng();
        SimulationResult sim2 = mc.generate(100, 252, 1, 1);

        std::vector<float> spots1 = sim1.pathbundle->unravel_spot();
        std::vector<float> spots2 = sim2.pathbundle->unravel_spot();

        REQUIRE(spots1.size() == spots2.size());

        
        for (size_t i = 1; i < spots1.size(); i++){
            REQUIRE(spots1[i] == spots2[i]);
        };
    }

        SECTION("Reset seed") {
        mc.configure(1);
        SimulationResult sim1 = mc.generate(100, 252, 1, 1);
        mc.reset_seed();
        SimulationResult sim2 = mc.generate(100, 252, 1, 1);

        std::vector<float> spots1 = sim1.pathbundle->unravel_spot();
        std::vector<float> spots2 = sim2.pathbundle->unravel_spot();

        REQUIRE(spots1.size() == spots2.size());

        
        bool identical = true;
        for (size_t i = 0; i < spots1.size(); ++i) {
            if (spots1[i] != spots2[i]) { identical = false; break; }
        }
        REQUIRE(identical == false);
    }

} 


TEST_CASE("Monte Carlo - Heston & Euler - basic usage"){

    Heston heston{0.02, 2, 0.05, 0.4, -0.5};
    EulerHeston eu_scheme(heston);
    MonteCarlo mc(eu_scheme);

    mc.configure(1);
    std::mt19937 rng;

    Path simulation= mc.simulate_path(100, 252, 1, rng, 0.2);

    REQUIRE(simulation.size() == 253);
    REQUIRE(simulation.end_state().spot() != 100);

}

TEST_CASE("Monte Carlo - Heston & Euler - Randomness"){

    Heston heston{0.02, 2, 0.05, 0.4, -0.5};
    EulerHeston eu_scheme(heston);

    MonteCarlo mc1(eu_scheme);
    MonteCarlo mc2(eu_scheme);
    MonteCarlo mc3(eu_scheme);

    std::mt19937 rng;

    mc1.configure(1);
    mc2.configure(1);
    mc3.configure(2);

    SimulationResult simulation1= mc1.generate(100, 252, 1, 1, 0.2);
    SimulationResult simulation2= mc2.generate(100, 252, 1, 1, 0.2);
    SimulationResult simulation3= mc3.generate(100, 252, 1, 1, 0.2);

    REQUIRE(simulation1.get_nsteps() == 252);
    REQUIRE(simulation1.avg_terminal_value() != 100);
    REQUIRE(simulation2.get_nsteps() == 252);
    REQUIRE(simulation2.avg_terminal_value() != 100);
    REQUIRE(simulation3.get_nsteps() == 252);
    REQUIRE(simulation3.avg_terminal_value() != 100);

    REQUIRE(mc1.get_seed() == 1);
    REQUIRE(mc2.get_seed() == 1);
    REQUIRE(mc3.get_seed() == 2);
    
    REQUIRE(simulation1.avg_terminal_value() == simulation2.avg_terminal_value());
    REQUIRE(simulation1.avg_terminal_value() != simulation3.avg_terminal_value());


}


TEST_CASE("Monte Carlo - QE - basic usage"){

    Heston heston{0.02, 2, 0.05, 0.4, -0.5};
    QE qe(heston);
    MonteCarlo mc(qe);

    mc.configure(1);
    std::mt19937 rng;

    Path simulation= mc.simulate_path(100, 252, 1, rng, 0.2);

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

    SimulationResult simulation1= mc1.generate(100, 252, 1, 1, 0.2);
    SimulationResult simulation2= mc2.generate(100, 252, 1, 1, 0.2);
    SimulationResult simulation3= mc3.generate(100, 252, 1, 1, 0.2);

    REQUIRE(simulation1.get_nsteps() == 252);
    REQUIRE(simulation1.avg_terminal_value() != 100);
    REQUIRE(simulation2.get_nsteps() == 252);
    REQUIRE(simulation2.avg_terminal_value() != 100);
    REQUIRE(simulation3.get_nsteps() == 252);
    REQUIRE(simulation3.avg_terminal_value() != 100);

    REQUIRE(mc1.get_seed() == 1);
    REQUIRE(mc2.get_seed() == 1);
    REQUIRE(mc3.get_seed() == 2);
    
    REQUIRE(simulation1.avg_terminal_value() == simulation2.avg_terminal_value());
    REQUIRE(simulation1.avg_terminal_value() != simulation3.avg_terminal_value());


}

TEST_CASE("Monte Carlo - QE - V > 0"){

    Heston heston{0.02, 2, 0.05, 0.4, -0.5};
    QE qe(heston);
    MonteCarlo mc(qe);

    mc.configure(1);
    std::mt19937 rng;

    Path simulation= mc.simulate_path(100, 1000, 1, rng, 0.2);

    REQUIRE(simulation.size() == 1001);
    
    for (State s : simulation){
        REQUIRE(s.vol().has_value());
        REQUIRE(s.vol().value() > 0);
    }

}


TEST_CASE("Monte Carlo - Parallelism") {

    Heston heston{0.02, 2, 0.05, 0.4, -0.5};
    EulerHeston qe(heston);
    
    MonteCarlo mc(qe);

    mc.configure(1, -1);
    
    SimulationResult res = mc.generate(100, 252,1, 20, 0.2);

    REQUIRE(res.avg_terminal_value() != 100);
    REQUIRE(res.get_npaths() == 20);
    REQUIRE(res.get_nsteps() == 252);
}


TEST_CASE("Dupire - Basic Usage") {

    std::vector<double> t {1,2,3, 4};
    std::vector<double> s {100,200,300};
    std::vector<double> vol{0.20, 0.19, 0.21,   
                            0.22, 0.20, 0.23,   
                            0.25, 0.24, 0.26,
                            0.27, 0.28, 0.29};

    LocalVolatilitySurface surface(t, s, vol);
    
    

    Dupire dupire(0.05, 0.02, std::make_shared<LocalVolatilitySurface>(surface));
    EulerDupire eu_dupire(dupire);
    
    MonteCarlo mc(eu_dupire);

    mc.configure(1, -1);
    
    SimulationResult res = mc.generate(100, 252,1, 20);

    REQUIRE(res.avg_terminal_value() != 100);
    REQUIRE(res.get_npaths() == 20);
    REQUIRE(res.get_nsteps() == 252);

}