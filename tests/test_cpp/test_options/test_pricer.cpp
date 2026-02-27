
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
#include "schemes/euler.h"
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

static double gamma(float S, float K, float T, float sigma, float r){

    double PI = 3.141592653589793238462;
    double d1 = (std::log(S / K) + (r + 0.5 * sigma * sigma) * T) / (sigma * std::sqrt(T));
    double Np_d1 = 1.0/(std::sqrt(2*PI)) * std::exp((-d1*d1)/2);
    return Np_d1/(S*sigma*std::sqrt(T));

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

    Instrument european_call(contract, std::make_unique<CallPayoff>());

    BlackScholes bs(r, sigma);
    Euler euler(std::make_shared<BlackScholes>(bs));

    MonteCarlo engine(euler);
    engine.configure(1, -1);

    MarketState state(100, r);

    Pricer p(state, 252, 10, std::make_shared<MonteCarlo>(engine));
    

};


TEST_CASE("Pricer : basic pricing with vol = 0") {

    double S0 = 100;
    double K = 100;
    double r = 0.02;
    double sigma = 0.0;
    double T = 1.0;

    OptionContract contract(K, T);

    Instrument european_call(contract, std::make_unique<CallPayoff>());

    BlackScholes bs(r, sigma);
    Euler euler(std::make_shared<BlackScholes>(bs));

    MonteCarlo engine(euler);
    engine.configure(1, -1);


    MarketState mstate(S0, r);

    
    Pricer pricer(mstate, 252, 10000,
                std::make_shared<MonteCarlo>(engine));
    


    double mc_price = pricer.compute_price(std::make_shared<Instrument>(european_call));

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

    Instrument european_put(contract, std::make_unique<PutPayoff>());

    BlackScholes bs(r, sigma);
    Euler euler(std::make_shared<BlackScholes>(bs));

    MonteCarlo engine(euler);
    engine.configure(1, -1);

    MarketState mstate(S0, r);
    Pricer pricer(mstate, 252, 10000,
                std::make_shared<MonteCarlo>(engine));
    



    double mc_price = pricer.compute_price(std::make_shared<Instrument>(european_put));

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

    Instrument call(contract, std::make_unique<CallPayoff>());

    BlackScholes bs(r, sigma);
    Euler euler(std::make_shared<BlackScholes>(bs));

    MonteCarlo engine(euler);
    engine.configure(1, -1);

    MarketState mstate(S0, r);
    Pricer pricer(mstate, 252, 10000, 
                std::make_shared<MonteCarlo>(engine));

    

    double mc_delta = pricer.compute_delta_bar(std::make_shared<Instrument>(call), 1e-4);

    double call_delta_val = call_delta(S0, K, T, sigma, r);

    REQUIRE(mc_delta == Catch::Approx(call_delta_val).epsilon(0.03));
};

TEST_CASE("Pricer : delta computation with randomness") {

    double S0 = 100;
    double K = 105;
    double r = 0.02;
    double sigma = 0.2;
    double T = 1.1;

    OptionContract contract(K, T);

    Instrument call(contract, std::make_unique<CallPayoff>());

    BlackScholes bs(r, sigma);
    Euler euler(std::make_shared<BlackScholes>(bs));
    MonteCarlo engine(euler);

    engine.configure(1, -1);
    MarketState mstate(S0, r);    
    Pricer pricer(mstate, 252, 10000,
                std::make_shared<MonteCarlo>(engine));
    


    double mc_delta1 = pricer.compute_delta_bar(std::make_shared<Instrument>(call),1e-4);
    double mc_delta2 = pricer.compute_delta_bar(std::make_shared<Instrument>(call),1e-4);
    

    REQUIRE(mc_delta1 == mc_delta2);
};

TEST_CASE("Pricer : gamma computation") {

    double S0 = 1.0;
    double K = 1.05;
    double r = 0.02;
    double sigma = 0.2;
    double T = 1.1;

    OptionContract contract(K, T);

    Instrument call(contract, std::make_unique<CallPayoff>());

    BlackScholes bs(r, sigma);
    Euler euler(std::make_shared<BlackScholes>(bs));

    MonteCarlo engine(euler);
    engine.configure(1, -1);

    MarketState mstate(S0, r);
    Pricer pricer(mstate, 252, 100000,
                std::make_shared<MonteCarlo>(engine));
    
    

    double mc_gamma = pricer.compute_gamma_bar(std::make_shared<Instrument>(call),1e-1);

    double call_gamma_val = gamma(S0, K, T, sigma, r);

    REQUIRE(mc_gamma == Catch::Approx(call_gamma_val).epsilon(0.05));
};


TEST_CASE("Pricer : batch pricing") {

    double S0 = 100.0;
    double r = 0.02;
    double sigma = 0.2;
    double T = 1.1;

    OptionContract call_102_con(102, T);
    OptionContract call_95_con(95, T);
    OptionContract put_105_con(105, T);
    OptionContract put_95_con(95, T);

    Instrument call_102(call_102_con, std::make_unique<CallPayoff>());
    Instrument call_95(call_95_con, std::make_unique<CallPayoff>());
    Instrument put_105(put_105_con, std::make_unique<PutPayoff>());
    Instrument put_95(put_95_con, std::make_unique<PutPayoff>());

    std::vector<std::shared_ptr<Instrument>> instruments{std::make_shared<Instrument>(call_102),
                                                         std::make_shared<Instrument>(call_95),
                                                         std::make_shared<Instrument>(put_105),
                                                         std::make_shared<Instrument>(put_95)};

    BlackScholes bs(r, sigma);
    Euler euler(std::make_shared<BlackScholes>(bs));

    MonteCarlo engine(euler);
    engine.configure(1, -1);

    MarketState mstate(S0, r);
    Pricer pricer(mstate, 252, 100000,
                std::make_shared<MonteCarlo>(engine));

    std::vector<double> prices = pricer.batch_price(instruments);

    double call_102_price = price_bs_call(S0, 102,T,  sigma,  r);
    double call_95_price = price_bs_call(S0, 95,T,  sigma,  r);
    double put_105_price = price_bs_put(S0, 105,T,  sigma,  r);
    double put_95_price = price_bs_put(S0, 95,T,  sigma,  r);
    
    REQUIRE(prices.size() == 4);

    REQUIRE(prices[0] == Catch::Approx(call_102_price).epsilon(0.05));
    REQUIRE(prices[1] == Catch::Approx(call_95_price).epsilon(0.05));
    REQUIRE(prices[2] == Catch::Approx(put_105_price).epsilon(0.05));
    REQUIRE(prices[3] == Catch::Approx(put_95_price).epsilon(0.05));
}

TEST_CASE("Pricer : options with different maturities") {

    double S0 = 100.0;
    double r = 0.02;
    double sigma = 0.2;
    double T = 1.1;

    OptionContract call_102_con(102, T);
    OptionContract call_95_con(95, T);
    OptionContract put_105_con(105, 1.2);
    OptionContract put_95_con(95, T);

    Instrument call_102(call_102_con, std::make_unique<CallPayoff>());
    Instrument call_95(call_95_con, std::make_unique<CallPayoff>());
    Instrument put_105(put_105_con, std::make_unique<PutPayoff>());
    Instrument put_95(put_95_con, std::make_unique<PutPayoff>());

    std::vector<std::shared_ptr<Instrument>> instruments{std::make_shared<Instrument>(call_102),
                                                         std::make_shared<Instrument>(call_95),
                                                         std::make_shared<Instrument>(put_105),
                                                         std::make_shared<Instrument>(put_95)};

    BlackScholes bs(r, sigma);
    Euler euler(std::make_shared<BlackScholes>(bs));

    MonteCarlo engine(euler);
    engine.configure(1, -1);

    MarketState mstate(S0, r);
    Pricer pricer(mstate, 252, 100000,
                std::make_shared<MonteCarlo>(engine));

    REQUIRE_THROWS_AS(pricer.batch_price(instruments), std::invalid_argument);
}