#pragma once

#include "schemes.hpp"
#include "models/dupire/dupire.hpp"
#include "types/state.hpp"

#include <optional>
#include <random>


// TO UPGRADE : create a generic Euler scheme for Dupire and BlackScholes
// need to refactor both classes to handle vol and spot compute 

class EulerDupire : public Scheme {

    public:
    EulerDupire(Dupire model);

    /**
    * @brief Creates the initial state at time 0
    * 
    *
    * @param S0 the spot at time 0
    * 
    * @return State : a State object representing the initial state
    */
    State init_state(double S0, std::optional<double> v0) const override;


    State step(const State& state, int i, float dt, std::mt19937& rng) const override;
    private:
    Dupire model_;
    float t_ = 0.f;

};
