#pragma once

#include "models/model.hpp"
#include "types/state.hpp"
#include <stdexcept>



/**
 * @brief Base class for the Black Scholes model
 *
 * This struct stores the main param of the BS model. Parameters 
 * include 'mu' (float) : the drift of the asset and 'sigma' (float) :
 * its volatility which must be greater or equal to zero. 
 * 
 * @param mu : the drift rate 
 * @param sigma : the volatility
 *
 */
struct BlackScholes : Model {
    BlackScholes(float mu, float sigma) : 
        mu(mu),
        sigma(sigma) {
            if (sigma < 0) throw std::invalid_argument("Black-Scholes Constructor : sigma must be strictly positive");
        }

    virtual ~BlackScholes() override = default;
    double drift(double t, const State& state) const override;
    double diffusion(double t, const State& state) const override;

    float mu; 
    float sigma; 

};
