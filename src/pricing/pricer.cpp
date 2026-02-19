
#include "pricing/pricer.h"
#include "instruments/instrument.h"
#include "types/marketstate.h"
#include "types/simulationresult.hpp"
#include <cmath>
#include <random>
#include <stdexcept>
#include <unordered_map>


Pricer::Pricer(Instrument& instrument, MonteCarlo& generator) :

    instrument_(instrument),
    generator_(generator)
{
}

double Pricer::compute_price(const MarketState& market_state, int n_steps, int n_paths) {

    if (n_steps <= 0) throw std::invalid_argument("Pricer::compute_price : n_steps must be superior to zero");
    if (n_paths <= 0) throw std::invalid_argument("Pricer::compute_price : n_paths must be superior to zero");

    double S0 = market_state.spot();
    double r = market_state.rf_rate();
    double T = instrument_.get_maturity();

    SimulationResult res = generator_.generate(S0,  n_steps, T, n_paths);
    double payoff = instrument_.compute_payoff(res);

    return payoff * std::exp(-r*T);

};

PricingResult Pricer::compute(const MarketState& market_state, int n_steps, int n_paths) {};

double Pricer::compute_delta_bar(const MarketState& market_state, int n_steps, int n_paths, double h) {

    if (n_steps <= 0) throw std::invalid_argument("Pricer::compute_delta_bar : n_steps must be superior to zero");
    if (n_paths <= 0) throw std::invalid_argument("Pricer::compute_delta_bar : n_paths must be superior to zero");
    if (h <= 0) throw std::invalid_argument("Pricer::compute_delta_bar : h must be superior to zero");


    double S0_p = market_state.spot() + h;
    double S0_m = market_state.spot() - h;
    double r = market_state.rf_rate();
    double T = instrument_.get_maturity();

    MonteCarlo local_generator = generator_; //to avoid changing the user's rng state

    local_generator.reset_rng();
    SimulationResult res_p = local_generator.generate(S0_p,  n_steps, T, n_paths);
    local_generator.reset_rng();
    SimulationResult res_m = local_generator.generate(S0_m,  n_steps, T, n_paths);
    double payoff_p = instrument_.compute_payoff(res_p);
    double payoff_m = instrument_.compute_payoff(res_m);
    double price_p = payoff_p * std::exp(-r*T);
    double price_m = payoff_m * std::exp(-r*T);

    return (price_p-price_m)/(2.0*h);
    
};

double Pricer::compute_gamma_bar(const MarketState& market_state, int n_steps, int n_paths, double h) {

    if (n_steps <= 0) throw std::invalid_argument("Pricer::compute_gamma_bar : n_steps must be superior to zero");
    if (n_paths <= 0) throw std::invalid_argument("Pricer::compute_gamma_bar : n_paths must be superior to zero");
    if (h <= 0) throw std::invalid_argument("Pricer::compute_gamma_bar : h must be superior to zero");

    double S0 = market_state.spot();
    double S0_p = market_state.spot() + h;
    double S0_m = market_state.spot() - h;
    double r = market_state.rf_rate();
    double T = instrument_.get_maturity();

    MonteCarlo local_generator = generator_; //to avoid changing the user's rng state

    local_generator.reset_rng();
    SimulationResult res_p = local_generator.generate(S0_p,  n_steps, T, n_paths);
    local_generator.reset_rng();
    SimulationResult res_m = local_generator.generate(S0_m,  n_steps, T, n_paths);
    local_generator.reset_rng();
    SimulationResult res = local_generator.generate(S0,  n_steps, T, n_paths);
    double payoff_p = instrument_.compute_payoff(res_p);
    double payoff_m = instrument_.compute_payoff(res_m);
    double payoff = instrument_.compute_payoff(res);
    double price_p = payoff_p * std::exp(-r*T);
    double price_m = payoff_m * std::exp(-r*T);
    double price = payoff *std::exp(-r*T);

    return (price_p - 2*price + price_m)/(h*h);

};