

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

    return State{S0, v*v};
}


State EulerDupire::step(const State& state, int i, float dt, std::mt19937& rng) const {


    if (dt <= 0) throw std::invalid_argument("EulerDupire::step : dt must be stricltly positive");

    std::normal_distribution<float> dist;
    float t = i*dt;
    float drift = model_.drift(t, state);
    float S = state.at(0);
    float diffusion = model_.diffusion(t, state);
    

    float Z = dist(rng);
    float St = S + (drift)*dt + diffusion*Z * std::sqrt(dt);

    return State{St, diffusion*diffusion/(S*S)};
};
