#include "schemes/eulerheston.hpp"
#include "models/heston/heston.hpp"
#include "schemes/schemes.hpp"
#include "types/state.hpp"


#include <optional>
#include <random>
#include <cmath>
#include <stdexcept>
#include <algorithm> 
#include <utility>



std::pair<double, double> EulerHeston::init_state(double S0, std::optional<double> v0) const {
    
    if (!v0.has_value()) throw std::invalid_argument("EulerHeston::init_state : intial state must receive a value for initial variance");
    if (v0.value() < 0) throw std::invalid_argument("EulerHeston::init_state : intial variance can't be negative");
    std::pair<double, double> res(S0, v0.value());
    return res;
}



std::pair<double, double> EulerHeston::step(const double S,
                            const double v, 
                            int i,
                            float dt, 
                            std::mt19937& rng) const 
                        
{

    if (dt <= 0) throw std::invalid_argument("EulerHeston::step : dt must be stricltly positive");
    float logS = std::log(S);    
    float V = (v*v);

    std::normal_distribution<float> dist;

    float Z = dist(rng);
    float Z_v = dist(rng);
    float Z_s = model.rho * Z_v + std::sqrt(1-model.rho*model.rho)*Z;

    const float v_plus = std::max(V, 0.f);
    const float sqrt_v = std::sqrt(v_plus);
    const float sqrt_dt = std::sqrt(dt);


    float Vt = V + model.kappa * (model.theta - v_plus) * dt + model.epsilon*sqrt_v * Z_v * sqrt_dt;
    float logSt = logS 
                    + (model.mu - 0.5*std::max(V, 0.f)) * dt 
                    + sqrt_v * sqrt_dt * Z_s;

    return std::pair<double, double>(std::exp(logSt), std::sqrt(Vt));
}