#pragma once

#include "models/model.hpp"
#include "schemes/schemes.hpp"
#include "types/state.hpp"
#include <memory>
#include <optional>
#include <random>



class Euler : public Scheme {

    public:
        Euler(std::shared_ptr<Model> model) : model_(model) {};
        virtual ~Euler() override = default;

        /**
        * @brief Creates the initial state at time 0
        * 
        *
        * @param S0 the spot at time 0
        * @param v0 optional : the volatility at time 0 - not required for Black Scholes
        * 
        * @return double : the value of the spot at time 0
        */
        std::pair<double, double> init_state(double S0, std::optional<double> v0) const override;
        
        /**
        * @brief Generates a new spot using Euler discretization
        * 
        * Simulates S_t+1 = a(S, t) * dt + b(S, t) * Z * dt 
        * with a() and b() the drift and diffusion process of the 
        * corresponding model
        *
        * @param state current state
        * @param i the current step of the generation process
        * @param dt the time step
        * @param rng the random number generator
        ° @param v the volatility
        * @return double : the next spot value
        */
        std::pair<double, double> step(const double S, const double v, int i, float dt, std::mt19937& rng) const override;


    private:

    std::shared_ptr<Model> model_;

};