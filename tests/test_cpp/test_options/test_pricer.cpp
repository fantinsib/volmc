
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>  
#include <memory>
#include <optional>
#include <stdexcept>
#include <cmath>
#include "pricing/pricer.h"
#include "models/black_scholes/black_scholes.hpp"
#include "options/options.hpp"
#include "payoff/payoff.h"
#include "instruments/instrument.h"
#include "schemes/eulerblackscholes.hpp"
#include "engine/montecarlo.hpp"
#include "types/marketstate.h"
#include "types/simulationresult.hpp"


static double price_bs_call(float S, float K, float T, float sigma, float r) {
    double d1 = (std::log(S / K) + (r + 0.5 * sigma * sigma) * T) / (sigma * std::sqrt(T));
    double d2 = d1 - sigma * std::sqrt(T);
    
    double N_d1 = 0.5 * (1.0 + std::erf(d1 / std::sqrt(2.0)));
    double N_d2 = 0.5 * (1.0 + std::erf(d2 / std::sqrt(2.0)));
    
    return (S * N_d1 - K * std::exp(-r * T) * N_d2);
}

static double call_delta(float S, float K, float T, float sigma, float r){
    double d1 = (std::log(S / K) + (r + 0.5 * sigma * sigma) * T) / (sigma * std::sqrt(T));
    return 0.5 * (1.0 + std::erf(d1 / std::sqrt(2.0)));

}

static double price_bs_put(float S, float K, float T, float sigma, float r) {
    double d1 = (std::log(S / K) + (r + 0.5 * sigma * sigma) * T) / (sigma * std::sqrt(T));
    double d2 = d1 - sigma * std::sqrt(T);
    
    double N_minus_d1 = 0.5 * (1.0 + std::erf(-d1 / std::sqrt(2.0)));
    double N_minus_d2 = 0.5 * (1.0 + std::erf(-d2 / std::sqrt(2.0)));
    
    return (-S * N_minus_d1 + K * std::exp(-r * T) * N_minus_d2);
}

TEST_CASE("Pricer : construction") {

    double S0 = 100;
    double K = 100;
    double r = 0.02;
    double sigma = 0.2;
    double T = 1.0;

    OptionContract contract(K, T);

    Instrument european_call(contract, std::make_unique<CallPayoff>(K));

    BlackScholes bs(r, sigma);
    EulerBlackScholes euler(bs);

    MonteCarlo engine(euler);
    engine.configure(1, -1);

    Pricer pricer(european_call, engine);
    

};


TEST_CASE("Pricer : basic pricing with vol = 0") {

    double S0 = 100;
    double K = 100;
    double r = 0.02;
    double sigma = 0.0;
    double T = 1.0;

    OptionContract contract(K, T);

    Instrument european_call(contract, std::make_unique<CallPayoff>(K));

    BlackScholes bs(r, sigma);
    EulerBlackScholes euler(bs);

    MonteCarlo engine(euler);
    engine.configure(1, -1);

    Pricer pricer(european_call, engine);

    MarketState mstate(S0, r);

    double mc_price = pricer.compute_price(mstate, 252, 10000);

    double bs_price = price_bs_call(S0, K, T, sigma, r);

    REQUIRE(mc_price == Catch::Approx(bs_price).epsilon(0.03));


}

TEST_CASE("Pricer : basic pricing with volatility") {

    double S0 = 100;
    double K = 105;
    double r = 0.02;
    double sigma = 0.2;
    double T = 1.1;

    OptionContract contract(K, T);

    Instrument european_put(contract, std::make_unique<PutPayoff>(K));

    BlackScholes bs(r, sigma);
    EulerBlackScholes euler(bs);

    MonteCarlo engine(euler);
    engine.configure(1, -1);

    Pricer pricer(european_put, engine);

    MarketState mstate(S0, r);

    double mc_price = pricer.compute_price(mstate, 252, 10000);

    double bs_price = price_bs_put(S0, K, T, sigma, r);

    REQUIRE(mc_price == Catch::Approx(bs_price).epsilon(0.03));
};

TEST_CASE("Pricer : delta computation") {

    double S0 = 100;
    double K = 105;
    double r = 0.02;
    double sigma = 0.2;
    double T = 1.1;

    OptionContract contract(K, T);

    Instrument call(contract, std::make_unique<CallPayoff>(K));

    BlackScholes bs(r, sigma);
    EulerBlackScholes euler(bs);

    MonteCarlo engine(euler);
    engine.configure(1, -1);

    Pricer pricer(call, engine);

    MarketState mstate(S0, r);

    double mc_delta = pricer.compute_delta_bar(mstate, 252, 10000,1e-4);

    double call_delta_val = call_delta(S0, K, T, sigma, r);

    REQUIRE(mc_delta == Catch::Approx(call_delta_val).epsilon(0.03));
};