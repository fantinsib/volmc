#include "models/black_scholes/black_scholes.hpp"




double BlackScholes::drift(double t, const double S) const {
    return S*mu;
};

double BlackScholes::diffusion(double t, const double S) const {
    return sigma*S;
};
