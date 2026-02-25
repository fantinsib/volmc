/*
 _            _          ___  _____ 
| |_ ___  ___| |_ ___   / _ \| ____|
| __/ _ \/ __| __/ __| | | | |  _|  
| ||  __/\__ \ |_\__ \ | |_| | |___ 
 \__\___||___/\__|___/  \__\_\_____|
*/


#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>  
#include <optional>
#include <stdexcept>
#include <utility>
#include "models/heston/heston.hpp"
#include "schemes/qe.hpp"


TEST_CASE("Scheme - QE"){

    SECTION("Constructor"){
    Heston heston{0.02, 2, 0.05, 0.4, -0.5};
    QE qe{heston};

    std::mt19937 rng;
    float dt = 0.1;


    std::pair<double, double> state = qe.step(100, 0.2,0, dt, rng);
    REQUIRE(state.first);
    REQUIRE(state.second);
    }

    SECTION("Constructor Error - Heston"){
{
    Heston heston{0.02, 2, 0.05, 0.4, -0.5};
    QE qe{heston};

    std::mt19937 rng;
    float dt = 0.1;
    std::pair<double, double> init{100, 0.2};

    std::pair<double, double> S = qe.step(init.first, init.second, 0, dt, rng);
    REQUIRE(S.first != 100.0);
    }

    }

    
    SECTION("Init state requires v0"){
        Heston heston{0.02, 2, 0.05, 0.4, -0.5};
        QE qe{heston};

        REQUIRE_THROWS_AS(qe.init_state(100.0f, std::nullopt), std::invalid_argument);
        std::pair<double, double> state = qe.init_state(100.0f, 0.2f);
        REQUIRE(state.first == Catch::Approx(100.0f));
        REQUIRE(state.second > 0);
    }

    SECTION("Psi threshold bounds"){
        Heston heston{0.02, 2, 0.05, 0.4, -0.5};

        REQUIRE_THROWS_AS((QE{heston, 0.9f}), std::invalid_argument);
        REQUIRE_THROWS_AS((QE{heston, 2.1f}), std::invalid_argument);

        QE qe_low{heston, 1.0f};
        QE qe_high{heston, 2.0f};
        REQUIRE(qe_low.psi_c() == Catch::Approx(1.0f));
        REQUIRE(qe_high.psi_c() == Catch::Approx(2.0f));
    }

    SECTION("Set psi threshold"){
        Heston heston{0.02, 2, 0.05, 0.4, -0.5};
        QE qe{heston};

        qe.set_psi_c(1.2f);
        REQUIRE(qe.psi_c() == Catch::Approx(1.2f));
        REQUIRE_THROWS_AS(qe.set_psi_c(0.5f), std::invalid_argument);
        REQUIRE_THROWS_AS(qe.set_psi_c(2.5f), std::invalid_argument);
    }

    SECTION("Missing volatility throws"){
        Heston heston{0.02, 2, 0.05, 0.4, -0.5};
        QE qe{heston};

        std::mt19937 rng;
        float dt = 0.1;
        std::pair<double, double> init(100, 0.3);

        REQUIRE_THROWS_AS(qe.init_state(init.first, std::nullopt), std::invalid_argument);
    }

}
