#include "schemes/eulerblackscholes.hpp"
#include "models/black_scholes/black_scholes.hpp"
#include "schemes/schemes.hpp"
#include "types/state.hpp"


#include <random>
#include <cmath>
#include <stdexcept>





State EulerBlackScholes::init_state(double S0,  std::optional<double> v0) const { 

    return State{S0};    
}


State EulerBlackScholes::step(const State& state, int i, float dt, std::mt19937& rng) const {

    if (dt <= 0) throw std::invalid_argument("EulerBlackScholes::step : dt must be stricltly positive");

    std::normal_distribution<double> dist;
    
    double logS = std::log(state.at(0));
    double Z = dist(rng);
    double logSt = logS + (model.mu - 0.5*model.sigma*model.sigma)*dt + model.sigma*Z * std::sqrt(dt);

    return State{std::exp(logSt)};
}

