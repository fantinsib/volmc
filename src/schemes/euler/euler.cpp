#include "schemes/euler.h"
#include "models/model.hpp"
#include "types/state.hpp"
#include <utility>




std::pair<double, double> Euler::init_state(double S0, std::optional<double> v0) const {
    if (v0.has_value()) return std::pair<double, double>(S0, v0.value());
    else return std::pair<double, double>(S0, 0.0); 
}

std::pair<double, double> Euler::step(const double S, const double v,int i, float dt, std::mt19937& rng) const {
    if (dt <= 0) throw std::invalid_argument("Euler::step : dt must be stricltly positive");

    std::normal_distribution<double> dist;
    double Z = dist(rng);
    double t = i * dt;
    double vt = model_->volatility(t,S);
    double St = S + model_->drift(t, S) * dt + model_->diffusion(t,S) *Z * std::sqrt(dt);

    return std::pair<double, double>(St, vt);


}