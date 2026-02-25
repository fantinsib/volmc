#pragma once



#include <optional>
#include <random>

#include "types/state.hpp"

class Scheme{

public:
    virtual ~Scheme() = default; 

    virtual std::pair<double, double> init_state(double S0, std::optional<double> v0) const = 0;
    /**
     * @brief Generate the next step in the stochastic process
     * 
     * @param S the current spot value
     * @param v the current volatility value
     * @param i the number of step
     * @param dt the time interval
     * @param rng the random number generator
     * @return std::pair<double, double> First value as spot, second as volatility
     */
    virtual std::pair<double, double> step(const double S, const double v, int i, float dt, std::mt19937& rng) const = 0;

};