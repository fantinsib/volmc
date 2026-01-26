
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
#include "engine/montecarlo.hpp"



TEST_CASE("Monte Carlo - BlackScholes & Euler - basic usage"){

    BlackScholes bs{0.02, 0.1};
    EulerBlackScholes eu_scheme(bs);
    MonteCarlo mc(eu_scheme);

    mc.set_seed(1);

    Path simulation= mc.simulate(100, 252, 1);

    REQUIRE(simulation.size() == 252);
    REQUIRE(simulation.end_state().spot() != 100);

}

TEST_CASE("Monte Carlo - BlackScholes & Euler - Randomness"){

    BlackScholes bs{0.02, 0.1};
    EulerBlackScholes eu_scheme(bs);
    MonteCarlo mc1(eu_scheme);
    MonteCarlo mc2(eu_scheme);
    MonteCarlo mc3(eu_scheme);

    mc1.set_seed(1);
    mc2.set_seed(1);
    mc3.set_seed(2);

    Path simulation1= mc1.simulate(100, 252, 1);
    Path simulation2= mc2.simulate(100, 252, 1);
    Path simulation3= mc3.simulate(100, 252, 1);

    REQUIRE(simulation1.size() == 252);
    REQUIRE(simulation1.end_state().spot() != 100);
    REQUIRE(simulation2.size() == 252);
    REQUIRE(simulation2.end_state().spot() != 100);
    REQUIRE(simulation3.size() == 252);
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

    mc.set_seed(1);

    Path simulation= mc.simulate(100, 252, 1, 0.2);

    REQUIRE(simulation.size() == 252);
    REQUIRE(simulation.end_state().spot() != 100);

}

TEST_CASE("Monte Carlo - Heston & Euler - Randomness"){

    Heston heston{0.02, 2, 0.05, 0.4, -0.5};
    EulerHeston eu_scheme(heston);
    
    MonteCarlo mc1(eu_scheme);
    MonteCarlo mc2(eu_scheme);
    MonteCarlo mc3(eu_scheme);

    mc1.set_seed(1);
    mc2.set_seed(1);
    mc3.set_seed(2);

    Path simulation1= mc1.simulate(100, 252, 1, 0.2);
    Path simulation2= mc2.simulate(100, 252, 1, 0.2);
    Path simulation3= mc3.simulate(100, 252, 1, 0.2);

    REQUIRE(simulation1.size() == 252);
    REQUIRE(simulation1.end_state().spot() != 100);
    REQUIRE(simulation2.size() == 252);
    REQUIRE(simulation2.end_state().spot() != 100);
    REQUIRE(simulation3.size() == 252);
    REQUIRE(simulation3.end_state().spot() != 100);

    REQUIRE(mc1.get_seed() == 1);
    REQUIRE(mc2.get_seed() == 1);
    REQUIRE(mc3.get_seed() == 2);
    
    REQUIRE(simulation1.end_state().spot() == simulation2.end_state().spot());
    REQUIRE(simulation1.end_state().spot() != simulation3.end_state().spot());


}
