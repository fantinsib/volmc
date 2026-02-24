
#include "models/ir_models/vasicek.h"





double Vasicek::drift(double t, const double S) const {
    return a_ *(b_ - S);
}

double Vasicek::diffusion(double t, const double S) const {
    return sigma_; 
}

double Vasicek::volatility(double t, const double S) const {
    return sigma_;
}