

#include "schemes/eulerdupire.hpp"
#include "models/dupire/dupire.hpp"
#include "types/state.hpp"
#include <cmath>
#include <stdexcept>
#include <random>


EulerDupire::EulerDupire(Dupire model) :
    model_(model) 
    {};


State EulerDupire::init_state(double S0, std::optional<double> v0) const{
    
    if (v0.has_value()) throw std::invalid_argument("EulerDupire::init_state : unexpected initial volatility");
    double v = model_.sigma(0.0, S0);
    return State{S0, v};
}


State EulerDupire::step(const State& state, int i, float dt, std::mt19937& rng) const {


    if (dt <= 0) throw std::invalid_argument("EulerDupire::step : dt must be stricltly positive");

    std::normal_distribution<float> dist;
    float t = i*dt;
    float drift = model_.drift();
    float S = state.spot();
    float sigma = model_.sigma(t, S);
    

    float logS = std::log(S);
    float Z = dist(rng);
    float logSt = logS + (drift - 0.5*sigma*sigma)*dt + sigma*Z * std::sqrt(dt);

    return State{std::exp(logSt), sigma};
};
