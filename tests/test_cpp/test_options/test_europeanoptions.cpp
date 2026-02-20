
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>  
#include <memory>
#include <optional>
#include <stdexcept>
#include <cmath>

#include "models/black_scholes/black_scholes.hpp"
#include "options/options.hpp"
#include "payoff/payoff.h"
#include "instruments/instrument.h"
#include "schemes/eulerblackscholes.hpp"
#include "engine/montecarlo.hpp"
#include "types/simulationresult.hpp"
#include <catch2/catch_test_macros.hpp>


static double price_bs_call(float S, float K, float T, float sigma, float r) {
    double d1 = (std::log(S / K) + (r + 0.5 * sigma * sigma) * T) / (sigma * std::sqrt(T));
    double d2 = d1 - sigma * std::sqrt(T);
    
    double N_d1 = 0.5 * (1.0 + std::erf(d1 / std::sqrt(2.0)));
    double N_d2 = 0.5 * (1.0 + std::erf(d2 / std::sqrt(2.0)));
    
    return (S * N_d1 - K * std::exp(-r * T) * N_d2);
}

static double price_bs_put(float S, float K, float T, float sigma, float r) {
    double d1 = (std::log(S / K) + (r + 0.5 * sigma * sigma) * T) / (sigma * std::sqrt(T));
    double d2 = d1 - sigma * std::sqrt(T);
    
    double N_minus_d1 = 0.5 * (1.0 + std::erf(-d1 / std::sqrt(2.0)));
    double N_minus_d2 = 0.5 * (1.0 + std::erf(-d2 / std::sqrt(2.0)));
    
    return (-S * N_minus_d1 + K * std::exp(-r * T) * N_minus_d2);
}

TEST_CASE("European option - basic construction"){

    SECTION("Call Option"){
        double K = 105;
        OptionContract contract(K, 0.8);
        
        Instrument european_call(contract, std::make_unique<CallPayoff>());
    }

    SECTION("Put Option"){
        double K = 105;
        OptionContract contract(K, 0.8);
        
        Instrument european_put(contract, std::make_unique<PutPayoff>());
    }
    
    
}

TEST_CASE("European option - basic pricing with vol = 0") {

    double S0 = 100;
    double K = 100;
    double r = 0.02;
    double sigma = 0.0;
    double T = 1.0;

    OptionContract contract(K, T);

    Instrument european_call(contract, std::make_unique<CallPayoff>());

    BlackScholes bs(r, sigma);
    EulerBlackScholes euler(bs);

    MonteCarlo engine(euler);
    engine.configure(1, -1);

    SimulationResult res = engine.generate(S0, 252, T, 100);

    double payoff = european_call.compute_payoff(res);

    REQUIRE(res.avg_terminal_value() == Catch::Approx(S0*std::exp(r*T)));

    REQUIRE(payoff == Catch::Approx(res.avg_terminal_value()-K));

    double bs_price = price_bs_call(S0, K, T, sigma, r);

    REQUIRE(payoff*std::exp(-r*T) == Catch::Approx(bs_price));


}

TEST_CASE("European option - basic pricing with volatility") {

    double S0 = 100;
    double K = 105;
    double r = 0.02;
    double sigma = 0.2;
    double T = 1.1;

    OptionContract contract(K, T);

    Instrument european_put(contract, std::make_unique<PutPayoff>());

    BlackScholes bs(r, sigma);
    EulerBlackScholes euler(bs);

    MonteCarlo engine(euler);
    engine.configure(1, -1);

    SimulationResult res = engine.generate(S0, 252, T, 10000);

    double payoff = european_put.compute_payoff(res);

    double bs_price = price_bs_put(S0, K, T, sigma, r);

    REQUIRE(payoff*std::exp(-r*T) == Catch::Approx(bs_price).epsilon(0.03));


}