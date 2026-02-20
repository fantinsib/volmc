#include "models/black_scholes/black_scholes.hpp"




double BlackScholes::drift(double t, const State &state) const {
    return state.at(0)*mu;
};

double BlackScholes::diffusion(double t, const State &state) const {
    return sigma*state.at(0);
};
