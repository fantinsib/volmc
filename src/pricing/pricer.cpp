
#include "pricing/pricer.h"
#include "engine/montecarlo.hpp"
#include "instruments/instrument.h"
#include "types/marketstate.h"
#include "types/simulationresult.hpp"
#include <cmath>
#include <memory>
#include <random>
#include <stdexcept>
#include <unordered_map>


Pricer::Pricer(const MarketState& marketstate, 
                size_t n_steps, size_t n_paths,
               std::shared_ptr<MonteCarlo> generator) :

    S0_(marketstate.spot()),
    r_(marketstate.rf_rate()),
    v0_(marketstate.vol()),
    n_steps_(n_steps),
    n_paths_(n_paths),
    generator_(generator)
{
}

double Pricer::compute_price(std::shared_ptr<Instrument> instrument) const {

    double T = instrument->get_maturity();


    double payoff;

    if (!v0_.has_value()) {
        SimulationResult res = generator_->generate_spot(S0_,  n_steps_, T, n_paths_);
        payoff = instrument->compute_payoff(res);
    }

    if (v0_.has_value()) {
        SimulationResult res = generator_->generate_spot(S0_,  n_steps_, T, n_paths_, v0_.value());
        payoff = instrument->compute_payoff(res);
    }


    return payoff * std::exp(-r_*T);

};

void Pricer::reconfigure(std::optional<size_t> n_steps, std::optional<size_t> n_paths, std::optional<MarketState> marketstate) {
    n_steps_ = n_steps.has_value() ? n_steps.value() : n_steps_;
    n_paths_ = n_paths.has_value() ? n_paths.value() : n_paths_;
    
    if (marketstate.has_value()){
        S0_ = marketstate->spot();
        r_ = marketstate->rf_rate();
        v0_ = marketstate->vol();
    }
}


PricingResult Pricer::compute(std::shared_ptr<Instrument> instrument, std::optional<double> h) const {};

double Pricer::compute_delta_bar(std::shared_ptr<Instrument> instrument, double h) {

    if (h <= 0) throw std::invalid_argument("Pricer::compute_delta_bar : h must be superior to zero");


    double S0_p = S0_ + h;
    double S0_m = S0_ - h;
    double T = instrument->get_maturity();

    MonteCarlo local_generator = *generator_; //to avoid changing the user's rng state

    local_generator.reset_rng();
    SimulationResult res_p = local_generator.generate_spot(S0_p,  n_steps_, T, n_paths_);
    local_generator.reset_rng();
    SimulationResult res_m = local_generator.generate_spot(S0_m,  n_steps_, T, n_paths_);
    double payoff_p = instrument->compute_payoff(res_p);
    double payoff_m = instrument->compute_payoff(res_m);
    double price_p = payoff_p * std::exp(-r_*T);
    double price_m = payoff_m * std::exp(-r_*T);

    return (price_p-price_m)/(2.0*h);
    
};

double Pricer::compute_gamma_bar(std::shared_ptr<Instrument> instrument, double h) {

    if (h <= 0) throw std::invalid_argument("Pricer::compute_gamma_bar : h must be superior to zero");

    double S0 = S0_;
    double S0_p = S0_ + h;
    double S0_m = S0_ - h;
    double r = r_;
    double T = instrument->get_maturity();

    MonteCarlo local_generator = *generator_; //to avoid changing the user's rng state

    local_generator.reset_rng();
    SimulationResult res_p = local_generator.generate_spot(S0_p,  n_steps_, T, n_paths_);
    local_generator.reset_rng();
    SimulationResult res_m = local_generator.generate_spot(S0_m,  n_steps_, T, n_paths_);
    local_generator.reset_rng();
    SimulationResult res = local_generator.generate_spot(S0,  n_steps_, T, n_paths_);
    double payoff_p = instrument->compute_payoff(res_p);
    double payoff_m = instrument->compute_payoff(res_m);
    double payoff = instrument->compute_payoff(res);
    double price_p = payoff_p * std::exp(-r*T);
    double price_m = payoff_m * std::exp(-r*T);
    double price = payoff *std::exp(-r*T);

    return (price_p - 2*price + price_m)/(h*h);

};