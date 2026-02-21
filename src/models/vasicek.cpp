
#include "models/ir_models/vasicek.h"





double Vasicek::drift(double t, const State &state) const {
    double r = state.at(0);
    return a_ *(b_ - r);
}

double Vasicek::diffusion(double t, const State &state) const {
    return sigma_; 
}