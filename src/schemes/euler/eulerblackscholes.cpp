#include "schemes/eulerblackscholes.hpp"
#include "models/black_scholes/black_scholes.hpp"
#include "schemes/schemes.hpp"
#include "types/state.hpp"


#include <random>
#include <cmath>
#include <stdexcept>





State EulerBlackScholes::init_state(float S0,  std::optional<float> v0) const { 

    State state{S0};
    return state;
    
}


State EulerBlackScholes::step(const State& state, int i, float dt, std::mt19937& rng) const {

    if (dt <= 0) throw std::invalid_argument("EulerBlackScholes::step : dt must be stricltly positive");

    std::normal_distribution<float> dist;
    
    float logS = std::log(state.spot());
    float Z = dist(rng);
    float logSt = logS + (model.mu - 0.5*model.sigma*model.sigma)*dt + model.sigma*Z * std::sqrt(dt);

    return State{std::exp(logSt)};
}

