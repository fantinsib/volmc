#include "schemes/euler.h"
#include "models/model.hpp"
#include "types/state.hpp"




State Euler::init_state(double S0, std::optional<double> v0) const {
    State state;
    if (v0.has_value()) {
        state.set(0, S0);
        state.set(1, v0.value());
    }
    else state.set(0, S0);
    
    return state;
}

State Euler::step(const State& state, int i, float dt, std::mt19937& rng) const {
    if (dt <= 0) throw std::invalid_argument("EulerBlackScholes::step : dt must be stricltly positive");

    std::normal_distribution<double> dist;
    double S = (state.at(0));
    double Z = dist(rng);
    double t = i * dt;
    double St = S + model_->drift(t, state)*dt + model_->diffusion(t, state)*Z * std::sqrt(dt);

    return State{St};


}