#pragma once

#include "models/model.hpp"
#include "schemes/schemes.hpp"
#include "types/state.hpp"
#include <random>



class Euler : public Scheme {

    public:
        Euler(Model& model) : model_(model) {};
        virtual ~Euler() override = default;

        /**
         * @brief Initialise the state 
         * 
         * @param S0 The initial spot value
         * @param v0 the initial variance value
         * @return State the initial state
         */
        State init_state(double S0, std::optional<double> v0) const override;
        State step(const State& state, int i, float dt, std::mt19937& rng) const override;




    private:

    const Model& model_;

};